#include "function.h"
#include "typescriptAPI_base.h"
#include "minsky.h"
#include "minsky.tcd"

#include "assetClass.tcd"
#include "bookmark.h"
#include "bookmark.tcd"
#include "cairoSurfaceImage.tcd"
#include "callableFunction.tcd"
#include "canvas.tcd"
#include "CSVDialog.tcd"
#include "CSVParser.tcd"
#include "constMap.tcd"
#include "dataSpecSchema.tcd"
#include "dimension.tcd"
#include "engNotation.tcd"
#include "evalGodley.tcd"
#include "eventInterface.tcd"
#include "fontDisplay.tcd"
#include "godleyIcon.tcd"
#include "godleyTab.tcd"
#include "godleyTable.tcd"
#include "godleyTableWindow.tcd"
#include "grid.tcd"
#include "group.tcd"
#include "handleLockInfo.tcd"
#include "hypercube.tcd"
#include "index.tcd"
#include "item.tcd"
#include "itemTab.tcd"
#include "lasso.tcd"
#include "noteBase.tcd"
#include "operation.tcd"
#include "operationType.tcd"
#include "pango.tcd"
#include "pannableTab.tcd"
#include "panopticon.tcd"
#include "parameterTab.tcd"
#include "plot.tcd"
#include "plotTab.tcd"
#include "plotWidget.tcd"
#include "polyRESTProcessBase.tcd"
#include "port.h"
#include "port.tcd"
#include "ravelState.tcd"
#include "renderNativeWindow.tcd"
#include "ravelWrap.tcd"
#include "rungeKutta.tcd"
#include "selection.tcd"
#include "simulation.tcd"
#include "slider.tcd"
#include "SVGItem.tcd"
#include "tensorInterface.tcd"
#include "tensorVal.tcd"
#include "units.tcd"
#include "variableInstanceList.h"
#include "variableInstanceList.tcd"
#include "variable.tcd"
#include "variablePane.tcd"
#include "variableTab.tcd"
#include "variableType.tcd"
#include "variableValue.tcd"
#include "variableValues.tcd"
#include "wire.tcd"
#include "xvector.tcd"

namespace classdesc_access
{
  // clobbers
  template <class T>
  struct access_typescriptAPI<classdesc::Exclude<T>>:
    public classdesc::NullDescriptor<classdesc::typescriptAPI_t> {};

  template <class T, class V, int N>
  struct access_typescriptAPI<ecolab::TCLAccessor<T,V,N>>:
    public classdesc::NullDescriptor<classdesc::typescriptAPI_t> {};

  template <>
  struct access_typescriptAPI<classdesc::PolyPackBase>:
    public classdesc::NullDescriptor<classdesc::typescriptAPI_t> {};
  
  template <class T>
  struct access_typescriptAPI<classdesc::PolyPack<T>>:
    public classdesc::NullDescriptor<classdesc::typescriptAPI_t> {};
  
  template <>
  struct access_typescriptAPI<std::vector<boost::any>>:
    public classdesc::NullDescriptor<classdesc::typescriptAPI_t> {};
}

namespace classdesc
{
  // dummies
  template <> string typeName<cairo_t>() {return "minsky__dummy";}
  template <> string typeName<cairo_surface_t>() {return "minsky__dummy";}
  
  template <class T>
  struct tn<boost::geometry::model::d2::point_xy<T>>
  {
    static string name() {return "minsky__dummy";}
  };
  template <>
  struct tn<RESTProcess_t>
  {
    static string name() {return "minsky__dummy";}
  };
  template <class C, class D>
  struct tn<std::chrono::time_point<C,D>>
  {
    static string name() {return "minsky__dummy";}
  };
  template <>
  struct tn<std::istream>
  {
    static string name() {return "minsky__dummy";}
  };
 
  // typescript has difficulties with specialised templates
  template <>
  struct tn<minsky::PannableTab<minsky::EquationDisplay>>
  {
    static string name() {return "EquationDisplay";}
  };
}

#include "minsky_epilogue.h"

namespace classdesc
{
  template <class T, class Base>
  typename enable_if<Not<is_map<Base>>, void>::T
  typescriptAPI(typescriptAPI_t& t, const string& d)
  {
    classdesc_access::access_typescriptAPI<Base>().template type<T>(t,d);
  }

  template <class T> void typescriptAPI(typescriptAPI_t& t, const string& d)
  {typescriptAPI<T,T>(t,d);}
}

namespace minsky
{
  Minsky& minsky()
  {
    static Minsky s_minsky;
    return s_minsky;
  }

  LocalMinsky::LocalMinsky(Minsky& minsky) {}
  LocalMinsky::~LocalMinsky() {}

  // GUI callback needed only to solve linkage problems
  void doOneEvent(bool idleTasksOnly) {}
}

using namespace classdesc;
using namespace std;
using namespace minsky;

void exportClass(const std::string& name, const minsky::typescriptAPI_ns::ClassType& klass)
{
      cout << "export class "+name+" extends "+(klass.super.empty()? "CppClass": klass.super)+" {\n";

      // properties
      for (auto& prop: klass.properties)
        cout << "  "<<prop.first <<": "<<prop.second.type<<";\n";

      // constructor
      if (!klass.super.empty())
        {
          cout << "  constructor(prefix: string|"<<klass.super<<"){\n";
          cout << "    if (typeof prefix==='string')\n";
          cout << "      super(prefix)\n";
          cout << "    else\n";
          cout << "      super((<"<<name<<">prefix).prefix)\n";
        }
      else
        {
          cout << "  constructor(prefix: string){\n";
          cout << "    super(prefix);\n";
        }
      for (auto& prop: klass.properties)
        {
          if (!prop.second.construction.empty())
            cout << "    this."<<prop.first<<"="<<prop.second.construction<<"\n";
          else
            cout << "    this."<<prop.first<<"=new "<<prop.second.type<<"(this.prefix+'/"<<prop.first<<"');\n"; 
        }
      cout << "  }\n";

      // methods
      for (auto& method: klass.methods)
        {
          cout << "  "<<method.first<<"(";
          for (size_t i=0; i<method.second.args.size(); ++i)
            {
              if (i>0) cout<<",";
              cout<<method.second.args[i].name<<": "<<method.second.args[i].type;
            }
          cout << "): "<<method.second.returnType<<" {return this.callMethod('"<<method.first<<"'";
          for (auto& arg: method.second.args)
            cout<<","<<arg.name;
          cout<<");}\n";
        }
      cout << "}\n\n";
}

int main()
{
  typescriptAPI_t api;
  typescriptAPI<Minsky>(api,"");

  // supporting types
  typescriptAPI<Bookmark>(api,"");
  typescriptAPI<civita::Dimension>(api,"");
  typescriptAPI<civita::Hypercube>(api,"");
  typescriptAPI<civita::Index>(api,"");
  typescriptAPI<civita::ITensor>(api,"");
  typescriptAPI<civita::XVector>(api,"");
  typescriptAPI<DataSpecSchema>(api,"");
  typescriptAPI<ecolab::Plot::LineStyle>(api,"");
  typescriptAPI<EngNotation>(api,"");
  typescriptAPI<GroupItems>(api,"");
  typescriptAPI<HandleLockInfo>(api,"");
  typescriptAPI<PannableTab<EquationDisplay>>(api,"");
  typescriptAPI<Port>(api,"");
  typescriptAPI<ravel::HandleState>(api,"");
  typescriptAPI<ravel::RavelState>(api,"");
  typescriptAPI<Units>(api,"");
  typescriptAPI<VariablePaneCell>(api,"");
  typescriptAPI<VariableValue>(api,"");

  // Item subclasses
  api["Group"].super="Item";
  typescriptAPI<GodleyIcon>(api,"");
  api["GodleyIcon"].super="Item";
  typescriptAPI<OperationBase>(api,"");
  api["Operation"].super="Item";
  typescriptAPI<PlotWidget>(api,"");
  api["PlotWidget"].super="Item";
  typescriptAPI<Ravel>(api,"");
  api["Ravel"].super="Item";
  typescriptAPI<VariableBase>(api,"");
  api["VariableBase"].super="Item";

  // to prevent Group recursively calling itself on construction
  api["Group"].properties.erase("parent");
  
  cout << "/*\nThis is a built file, please do not edit.\n";
  cout << "See RESTService/typescriptAPI for more information.\n*/\n\n";
  cout << "import {CppClass, Sequence, Container, Map, Pair} from './backend';\n\n";

  // dummy types
  cout << "class minsky__dummy {}\n";
  cout << "class classdesc__json_pack_t {}\n";
  cout << "class classdesc__pack_t {}\n";
  cout << "class classdesc__TCL_obj_t {}\n";
  cout << "class ecolab__cairo__Surface {}\n";
  cout << "class ecolab__Pango {}\n";
  cout << "class ecolab__TCL_args {}\n";
  cout<<endl;
  
  // export Item first, as it is the base of many
  exportClass("Item",api["Item"]);
  for (auto& i: api)
    if (i.first!="Item")
      exportClass(i.first, i.second);

  cout << "export var minsky=new Minsky('/minsky');\n";
}
