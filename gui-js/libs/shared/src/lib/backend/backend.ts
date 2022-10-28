import * as JSON5 from 'json5';

export class CppClass
{
  public static backend : (...args)=>any; // pass command to destination: see backend-init and electron.service
  public static record=(cmd: string)=>{}; // recording support: see RecordingsManager
  protected prefix: string;
  constructor(prefix: string) {this.prefix=prefix;}
  protected callMethod(method: string,...args)
  {
    return CppClass.backend(`${this.prefix}/${method}`, ...args);
  }
  public properties(...args) {return CppClass.backend(this.prefix, ...args);}
  public $list(): string[] {return this.callMethod("@list");}
  /// returns if this proxy object and x refer to the same backend object
  public equal(x: CppClass): boolean {return this.prefix===x.prefix;}
  public prefix$(): string {return this.prefix;}
};


export class Pair<Key, Value> {
  first: Key;
  second: Value;
  constructor(key: Key, value: Value) {
    this.first=key;
    this.second=value;
  }
};

export class Map<Key, Value> extends CppClass
{
  valueType: any;  // stash a reference to the actual type here, for use in a new expression
  constructor(prefix: string, valueType: any=null) {super(prefix); this.valueType=valueType;}
  elem(key: Key) {
    const cmd=`${this.prefix}/@elem/${JSON5.stringify(key)}/second`;
    // if proxy type provided, instantiate that, otherwise return the current value
    return new Pair<Key,Value>
      (key,this.valueType? new this.valueType(cmd): this.callMethod(cmd));
  }
  insert(key: Key, value: Value) {this.callMethod("@insert",{first: key, second:value});}
  erase(key: Key) {this.callMethod("@erase",key);}
  size(): number {return this.callMethod("@size");}
  keys(): Key[] {return this.callMethod("@keys");}
};

export class Container<Key,Value=Key> extends CppClass
{
  type: any;
  constructor(prefix: string, type: any=null) {super(prefix); this.type=type}
  elem(key: Key) {
    // if proxy type provided, instantiate that, otherwise return the current value
    const cmd=`${this.prefix}/@elem/${JSON5.stringify(key)}`;
    return this.type? new this.type(cmd): this.callMethod(cmd);
  }
  insert(key: Key) {this.callMethod("@insert",key);}
  erase(key: Key) {this.callMethod("@erase",key);}
  size(): number {return this.callMethod("@size");}
  properties(...args): Value[] {return super.properties(...args) as Value[];}
};

export class Sequence<Value> extends Container<number,Value>
  {
    constructor(prefix: string, type: any=null) {super(prefix,type);}
  }

