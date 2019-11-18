/*
  @copyright Steve Keen 2019
  @author Russell Standish
  This file is part of Minsky.

  Minsky is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Minsky is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Minsky.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef RESTPROCESS_H

namespace classdesc
{
  template <>
  struct tn<cairo_t>
  {
    static string name() {return "cairo_t";}
  };
  template <>
  struct tn<cairo_surface_t>
  {
    static string name() {return "cairo_surface_t";}
  };
}

namespace classdesc_access
{
  namespace cd=classdesc;
  template <class T, class G, class S>
  struct access_RESTProcess<ecolab::Accessor<T,G,S>>
  {
    template <class U>
    void operator()(cd::RESTProcess_t& r, const cd::string& d, U& a)
    {
      ::RESTProcess(r,d,a.g);
      ::RESTProcess(r,d,a.s);
    }
  };

  template <class T>
  struct access_RESTProcess<ecolab::array<T>>
  {
    template <class U>
    void operator()(cd::RESTProcess_t& r, const cd::string& d, U& a)
    {
      r.add(d,new cd::RESTProcessSequence<ecolab::array<T>>(a));
    }
  };

  template <> struct access_json_pack<cd::TCL_obj_t>:
    public cd::NullDescriptor<cd::json_pack_t> {};
  template <> struct access_json_unpack<cd::TCL_obj_t>:
    public cd::NullDescriptor<cd::json_unpack_t> {};

  template <> struct access_json_pack<ecolab::cairo::Surface>:
    public cd::NullDescriptor<cd::json_pack_t> {};
  template <> struct access_json_unpack<ecolab::cairo::Surface>:
    public cd::NullDescriptor<cd::json_unpack_t> {};
  template <> struct access_RESTProcess<ecolab::cairo::Surface>:
    public cd::NullDescriptor<cd::RESTProcess_t> {};

  template <> struct access_json_pack<ecolab::CairoSurface>:
    public cd::NullDescriptor<cd::json_pack_t> {};
  template <> struct access_json_unpack<ecolab::CairoSurface>:
    public cd::NullDescriptor<cd::json_unpack_t> {};
  template <> struct access_RESTProcess<ecolab::CairoSurface>:
    public cd::NullDescriptor<cd::RESTProcess_t> {};

#ifdef XVECTOR_H
  template <>
  struct access_RESTProcess<minsky::XVector>: public classdesc::NullDescriptor<cd::RESTProcess_t> {};
  template <>
  struct access_json_pack<minsky::XVector>: public classdesc::NullDescriptor<cd::json_pack_t> {};
  template <>
  struct access_json_unpack<minsky::XVector>: public classdesc::NullDescriptor<cd::json_unpack_t> {};
#endif
  template <>
  struct access_json_pack<cd::RESTProcess_t>: public classdesc::NullDescriptor<cd::json_pack_t> {};
  template <>
  struct access_json_unpack<cd::RESTProcess_t>: public classdesc::NullDescriptor<cd::json_unpack_t> {};
  
}
#endif

#ifdef JSON_PACK_BASE_H
namespace classdesc_access
{
  template <class T, class G, class S>
  struct access_json_pack<ecolab::Accessor<T,G,S>>
  {
    template <class U>
    void operator()(cd::json_pack_t& j, const cd::string& d, U& a)
    {
      j<<a();
    }
  };

  template <class T, class G, class S>
  struct access_json_unpack<ecolab::Accessor<T,G,S>>
  {
    template <class U>
    void operator()(cd::json_unpack_t& j, const cd::string& d, U& a)
    {
      T x;
      j>>x;
      a(x);
    }
  };

  template <>
  struct access_json_pack<ecolab::TCL_args>: public cd::NullDescriptor<cd::json_pack_t> {};
  template <>
  struct access_json_unpack<ecolab::TCL_args>: public cd::NullDescriptor<cd::json_unpack_t> {};
}

#endif

#include <ecolab_epilogue.h>
