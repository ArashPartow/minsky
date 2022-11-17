import { AfterViewInit, Component, OnDestroy, OnInit, ElementRef, ViewChild } from '@angular/core';
import { AbstractControl, FormControl, FormGroup } from '@angular/forms';
import { ActivatedRoute } from '@angular/router';
import { ElectronService } from '@minsky/core';
import {
  dateTimeFormats,
  importCSVerrorMessage,
  importCSVvariableName,
  VariableBase,
  VariableValue,
} from '@minsky/shared';
import { MessageBoxSyncOptions } from 'electron/renderer';
import { AutoUnsubscribe } from 'ngx-auto-unsubscribe';

enum ColType {
  axis="axis",
  data="data",
  ignore="ignore"
};

function separatorToChar(sep: string): string {
  switch(sep) {
  case 'space': return ' ';
  case 'tab': return '\t';
  default: return sep;
  }
}

function separatorFromChar(sep: string): string {
  switch(sep) {
  case ' ': return 'space';
  case '\t': return 'tab';
  default: return sep;
  }
}

class Dimension
{
  type: string;
  units: string;
};

@AutoUnsubscribe()
@Component({
  selector: 'minsky-import-csv',
  templateUrl: './import-csv.component.html',
  styleUrls: ['./import-csv.component.scss'],
})
export class ImportCsvComponent implements OnInit, AfterViewInit, OnDestroy {
  form: FormGroup;

  itemId: string;
  systemWindowId: number;
  isInvokedUsingToolbar: boolean;
  valueId: string;
  variableValuesSubCommand: VariableValue;
  timeFormatStrings = dateTimeFormats;
  parsedLines: string[][] = [];
  csvCols: any[];
  selectedHeader = 0;
  selectedRow = -1;
  selectedCol = -1;
  colType: Array<ColType> = [];
  // per column dimension names and dimensions
  dimensionNames: string[]=[];
  dimensions: Dimension[]=[];
  dialogState: any;
  @ViewChild('checkboxRow') checkboxRow: ElementRef<HTMLCollection>;
  @ViewChild('importCsvCanvasContainer') inputCsvCanvasContainer: ElementRef<HTMLElement>;

  public get url(): AbstractControl {
    return this.form.get('url');
  }
  public get columnar(): AbstractControl {
    return this.form.get('columnar');
  }
  public get separator(): AbstractControl {
    return this.form.get('separator');
  }
  public get decSeparator(): AbstractControl {
    return this.form.get('decSeparator');
  }
  public get escape(): AbstractControl {
    return this.form.get('escape');
  }
  public get quote(): AbstractControl {
    return this.form.get('quote');
  }
  public get mergeDelimiters(): AbstractControl {
    return this.form.get('mergeDelimiters');
  }
  public get missingValue(): AbstractControl {
    return this.form.get('missingValue');
  }
  public get duplicateKeyAction(): AbstractControl {
    return this.form.get('duplicateKeyAction');
  }
  public get horizontalDimName(): AbstractControl {
    return this.form.get('horizontalDimName');
  }
  public get horizontalDimension(): AbstractControl {
    return this.form.get('horizontalDimension');
  }
  public get type(): AbstractControl {
    return this.horizontalDimension.get('type');
  }
  public get format(): AbstractControl {
    return this.horizontalDimension.get('units');
  }

  constructor(
    private electronService: ElectronService,
    private route: ActivatedRoute
  ) {
    this.route.queryParams.subscribe((params) => {
      this.itemId = params.itemId;
      this.systemWindowId = params.systemWindowId;
      this.isInvokedUsingToolbar = params.isInvokedUsingToolbar;
    });

    this.form = new FormGroup({
      columnar: new FormControl(false),
      decSeparator: new FormControl('.'),
      duplicateKeyAction: new FormControl('throwException'),
      escape: new FormControl(''),
      horizontalDimName: new FormControl('?'),
      mergeDelimiters: new FormControl(false),
      missingValue: new FormControl(NaN),
      quote: new FormControl('"'),
      separator: new FormControl(','),

      // review
      url: new FormControl(''),
      horizontalDimension: new FormGroup({
        type: new FormControl('string'),
        units: new FormControl(''),
      }),
    });
  }

  ngOnInit() {
    // ??
    this.form.valueChanges.subscribe(async (_form) => {
      if (this.url === _form.url) {
        await this.parseLines();
      }
    });
  }

  ngAfterViewInit() {
    (async () => {
      this.valueId = await this.getValueId();
      this.variableValuesSubCommand = this.electronService.minsky.variableValues.elem(this.valueId).second;
      
      
      await this.getCSVDialogSpec();
      this.updateForm();
      this.selectedHeader = this.dialogState.spec.headerRow as number;
      this.load();
      this.selectRowAndCol(this.dialogState.spec.dataRowOffset, this.dialogState.spec.dataColOffset);
    })();
  }
  ngAfterViewChecked() 	{
    // set state of dimension controls to reflect dialogState
    if (this.inputCsvCanvasContainer)
    {
      var table=this.inputCsvCanvasContainer.nativeElement.children[0] as HTMLTableElement;
      if (!table) return;
      for (var i=0; i<this.colType.length; ++i)
        {
            var colType=table.rows[0].cells[i+1]?.children[0] as HTMLInputElement;
            if (colType)
                colType.value=this.colType[i];
            if (this.colType[i]===ColType.axis)
            {
              var type=table.rows[1].cells[i+1]?.children[0] as HTMLSelectElement;
              var dimension=this.dimensions[i];
              if (!dimension) dimension={type: "string", units: ""};
              type.value=dimension.type;
              var format=table.rows[2].cells[i+1].children[0] as HTMLInputElement;
              format.value=dimension.units;
              var name=table.rows[3].cells[i+1].children[0] as HTMLInputElement;
              name.value=this.dimensionNames[i];
            }
        }
    }
  }
  
  updateForm() {
    this.url.setValue(this.dialogState.url);
    
    this.columnar.setValue(this.dialogState.spec.columnar);
    this.decSeparator.setValue(this.dialogState.spec.decSeparator);
    this.duplicateKeyAction.setValue(this.dialogState.spec.duplicateKeyAction);
    this.escape.setValue(this.dialogState.spec.escape);
    this.horizontalDimName.setValue(this.dialogState.spec.horizontalDimName);
    this.mergeDelimiters.setValue(this.dialogState.spec.mergeDelimiters);
    this.missingValue.setValue(this.dialogState.spec.missingValue);
    this.quote.setValue(this.dialogState.spec.quote);
    this.separator.setValue(separatorFromChar(this.dialogState.spec.separator));
    this.horizontalDimension.setValue({
      type: this.dialogState.spec.horizontalDimension.type,
      units:this.dialogState.spec.horizontalDimension.units
    });
  }

  async getValueId() {
    return new VariableBase(this.electronService.minsky.namedItems.elem(this.itemId).second).valueId();
  }

  async selectFile() {
    const filePath = await this.electronService.openFileDialog({
      filters: [
        { extensions: ['csv'], name: 'CSV' },
        { extensions: ['*'], name: 'All Files' },
      ],
    });

    if (!filePath) {return;}
    this.url.setValue(filePath);
    this.dialogState.url=filePath;
  }

  async load() {
    const fileUrlOnServer = await this.variableValuesSubCommand.csvDialog.url();
    const fileUrl = this.url.value;

    if (fileUrl !== fileUrlOnServer) {
      await this.variableValuesSubCommand.csvDialog.url(fileUrl);
      await this.variableValuesSubCommand.csvDialog.guessSpecAndLoadFile();
    } else {
      await this.variableValuesSubCommand.csvDialog.loadFile();
    }

    await this.parseLines();
  }

  async getCSVDialogSpec() {
    this.variableValuesSubCommand.csvDialog.spec.toSchema();
    this.dialogState = await this.variableValuesSubCommand.csvDialog.properties() as Record<string, unknown>;
  }

  async parseLines() {
    this.parsedLines = await this.variableValuesSubCommand.csvDialog.parseLines() as string[][];

    this.csvCols = new Array(this.parsedLines[0]?.length);
    this.colType = new Array(this.parsedLines[0]?.length).fill("ignore");
    for (var i in this.dialogState.spec.dimensionCols as Array<number>)
    {
      var col=this.dialogState.spec.dimensionCols[i];
      if (col<this.colType.length)
        this.colType[col]=ColType.axis;
      this.dimensionNames[col]=this.dialogState.spec.dimensionNames[i] as string;
      this.dimensions[col]=this.dialogState.spec.dimensions[i] as Dimension;
    }
    if (this.dialogState.spec.dataCols)
      for (var i in this.dialogState.spec.dataCols as Array<number>)
    {
      var col=this.dialogState.spec.dimensionCols[i];
      if (col<this.colType.length)
        this.colType[col]=ColType.data;
    }
    else
      // emulate old behaviour, if no datacols specified, all columns to the right of dataColOffset are data
      for (var ii=this.dialogState.dataColOffset as number; ii<this.colType.length; ++ii)
        this.colType[ii]=ColType.data;
    
  }
  
  async selectHeader(index: number) {
    this.selectedHeader = index;
    this.dialogState.spec.headerRow = this.selectedHeader;
  }

  async selectRowAndCol(rowIndex: number, colIndex: number) {
    this.selectedRow = rowIndex;
    this.dialogState.spec.dataRowOffset = rowIndex;

    this.selectedCol = colIndex;
    this.dialogState.spec.dataColOffset = colIndex;

    if (!this.parsedLines.length) return;
    for (let i = 0; i<this.parsedLines[0].length; i++)
      //for (let i = this.selectedCol; this.dialogState.spec.columnar? i<this.selectedCol + 1: i < this.parsedLines.length; i++) {
      if (i<this.selectedCol) {
        if (this.colType[i]==ColType.data)
          this.colType[i]=ColType.ignore;
      } else if (i===this.selectedCol || !this.columnar.value)
        this.colType[i]=ColType.data;
    else
      this.colType[i]=ColType.ignore;
    this.ngAfterViewChecked();
  }

  getColorForCell(rowIndex: number, colIndex: number) {
    if (colIndex>=this.colType.length) return "red";

    if (this.selectedHeader === rowIndex)  // header row
        switch (this.colType[colIndex]) {
        case 'data': return "blue";
        case 'axis': return "green";
        case "ignore": return "red";
        }
    else if (this.selectedRow >= 0 && this.selectedRow > rowIndex)
      return "red"; // ignore commentary at beginning of file
    else
      switch (this.colType[colIndex]) {
      case 'data': return "black";
      case 'axis': return "blue";
      case "ignore": return "red";
      }
  }

  setColType(column: number, type: ColType) {
    this.colType[column]=type;
    if (!this.dimensionNames[column])
      this.dimensionNames[column]=this.parsedLines[this.selectedHeader][column];
    if (!this.dimensions[column])
      this.dimensions[column]={type:"string",units:""} as Dimension;
  }

  async handleSubmit() {
    const {
      columnar,
      decSeparator,
      duplicateKeyAction,
      escape,
      horizontalDimName,
      mergeDelimiters,
      missingValue,
      quote,
      separator,
      horizontalDimension,
    } = this.form.value;

    this.dialogState.spec.dimensionCols=[];
    this.dialogState.spec.dataCols=[];
    this.dialogState.spec.dimensionNames=[];
    this.dialogState.spec.dimensions=[];
    for (let i=0; i<this.colType.length; ++i) 
      switch (this.colType[i]) {
      case ColType.axis: 
        this.dialogState.spec.dimensionCols.push(i);
        this.dialogState.spec.dimensionNames.push(this.dimensionNames[i]);
        this.dialogState.spec.dimensions.push(this.dimensions[i]);
        break;
      case ColType.data:
        this.dialogState.spec.dataCols.push(i);
        break;
    }

    const spec = {
      ...this.dialogState.spec,
      columnar,
      decSeparator,
      duplicateKeyAction,
      escape,
      horizontalDimName,
      mergeDelimiters,
      missingValue,
      quote,
      separator: separatorToChar(separator),
      horizontalDimension,
    };

    let v=new VariableBase(this.electronService.minsky.canvas.item);
    // returns an error message on error
    const res = await v.importFromCSV(this.url.value,spec) as unknown as string;

    if (res === importCSVerrorMessage) {
      const positiveResponseText = 'Yes';
      const negativeResponseText = 'No';

      const options: MessageBoxSyncOptions = {
        buttons: [positiveResponseText, negativeResponseText],
        message: 'Something went wrong... Do you want to generate a report?',
        title: 'Generate Report ?',
      };

      const index = await this.electronService.showMessageBoxSync(options);

      if (options.buttons[index] === positiveResponseText) {
        await this.doReport();
      }
      this.closeWindow();

      return;
    }

    const currentItemId = await v.id();
    const currentItemName = await v.name();

    if (
      this.isInvokedUsingToolbar &&
      currentItemId === this.itemId &&
      currentItemName === importCSVvariableName &&
      this.url.value
    ) {
      const path = this.url.value as string;
      const pathArray = this.electronService.isWindows() ? path.split(`\\`) : path.split(`/`);

      const fileName = pathArray[pathArray.length - 1].split(`.`)[0];

      await this.electronService.minsky.canvas.renameItem(fileName);
    }

    this.closeWindow();
  }

  async doReport() {
    const filePathWithoutExt = (this.url.value as string)
      .toLowerCase()
      .split('.csv')[0];

     const filePath = await this.electronService.saveFileDialog({
      defaultPath: `${filePathWithoutExt}-error-report.csv`,
      title: 'Save report',
      properties: ['showOverwriteConfirmation', 'createDirectory'],
      filters: [{ extensions: ['csv'], name: 'CSV' }],
    });
    if (!filePath) return;

    await this.variableValuesSubCommand.csvDialog.reportFromFile(this.url.value,filePath);
    return;
  }

  closeWindow() {this.electronService.closeWindow();}

  // eslint-disable-next-line @typescript-eslint/no-empty-function,@angular-eslint/no-empty-lifecycle-method
  ngOnDestroy() {}
}
