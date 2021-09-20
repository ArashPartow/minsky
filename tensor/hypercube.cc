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

#include "hypercube.h"
#include <error.h>
#include <set>

using namespace std;

namespace civita
{
  std::vector<unsigned> Hypercube::dims() const
  {
    std::vector<unsigned> d;
    for (auto& i: xvectors) d.push_back(i.size());
    return d;
  }
  
  std::vector<string> Hypercube::dimLabels() const
  {
    std::vector<std::string> l;			  
    for (auto& i: xvectors) l.push_back(static_cast<std::string>(i.name));
    return l;
  }      

  const std::vector<unsigned>& Hypercube::dims(const std::vector<unsigned>& d) {
    xvectors.clear();
    for (size_t i=0; i<d.size(); ++i)
      {
        xvectors.emplace_back(std::to_string(i));
        xvectors.back().dimension.type=Dimension::value;
        for (size_t j=0; j<d[i]; ++j)
          xvectors.back().emplace_back(double(j));
      }
    return d;
  }

  size_t Hypercube::numElements() const
    {
      size_t s=1;
      for (auto& i: xvectors)
        s*=i.size();
      return s;
    }

  double Hypercube::logNumElements() const
  {
    double r=0;
    for (auto& i: xvectors)
      r+=log(i.size());
    return r;
  }
  
  /// split lineal index into components along each dimension
  vector<size_t> Hypercube::splitIndex(size_t i) const
  {
    std::vector<size_t> splitIndex;
    splitIndex.reserve(xvectors.size());
    for (auto& xv: xvectors)
      {
        auto res=div(ssize_t(i),ssize_t(xv.size()));
        splitIndex.push_back(res.rem);
        i=res.quot;
      }
    return splitIndex;
  }
}

