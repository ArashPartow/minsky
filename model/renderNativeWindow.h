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

#ifndef RENDER_NATIVE_WINDOW_H
#define RENDER_NATIVE_WINDOW_H

#include <cairoSurfaceImage.h>

namespace minsky
{  
  class WindowInformation;
  class RenderNativeWindow : public ecolab::CairoSurface
  {
  private:
    CLASSDESC_ACCESS(RenderNativeWindow); 
    std::shared_ptr<WindowInformation> winInfoPtr;

  public:    
    void resizeWindow(int offsetLeft, int offsetTop, int childWidth, int childHeight);
    void renderFrame(unsigned long parentWindowId, int offsetLeft, int offsetTop, int childWidth, int childHeight);
  };
} // namespace minsky

#include "renderNativeWindow.cd"
#endif
