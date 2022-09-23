/*
  @copyright Steve Keen 2021
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

#define OPNAMEDEF
#include "operation.h"
#include "lasso.h"
#include "item.rcd"
#include "operation.rcd"
#include "operationType.rcd"
#include "minsky_epilogue.h"

#define DEFOP(type) CLASSDESC_ACCESS_EXPLICIT_INSTANTIATION(minsky::Operation<minsky::OperationType::type>);

DEFOP(constant)
DEFOP(time)
DEFOP(integrate)
DEFOP(differentiate)
DEFOP(data)
DEFOP(ravel)
DEFOP(euler)
DEFOP(pi)
DEFOP(zero)
DEFOP(one)
DEFOP(inf)
DEFOP(percent)
DEFOP(add)
DEFOP(subtract)
DEFOP(multiply)
DEFOP(divide)
DEFOP(min)
DEFOP(max)
DEFOP(and_)
DEFOP(or_)
DEFOP(log)
DEFOP(pow)
DEFOP(polygamma)
DEFOP(lt)
DEFOP(le)
DEFOP(eq)
DEFOP(userFunction)
DEFOP(copy)
DEFOP(sqrt)
DEFOP(exp)
DEFOP(ln)
DEFOP(sin)
DEFOP(cos)
DEFOP(tan)
DEFOP(asin)
DEFOP(acos)
DEFOP(atan)
DEFOP(sinh)
DEFOP(cosh)
DEFOP(tanh)
DEFOP(abs)
DEFOP(floor)
DEFOP(frac)
DEFOP(not_)
DEFOP(Gamma)
DEFOP(fact)
DEFOP(sum)
DEFOP(product)
DEFOP(infimum)
DEFOP(supremum)
DEFOP(any)
DEFOP(all)
DEFOP(infIndex)
DEFOP(supIndex)
DEFOP(runningSum)
DEFOP(runningProduct)
DEFOP(difference)
DEFOP(innerProduct)
DEFOP(outerProduct)
DEFOP(index)
DEFOP(gather)
DEFOP(numOps)
CLASSDESC_ACCESS_EXPLICIT_INSTANTIATION(minsky::NamedOp);
