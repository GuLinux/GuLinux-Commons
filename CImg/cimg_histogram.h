/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2016  <copyright holder> <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef GULINUX_COMMONS_CIMG_HISTOGRAM
#define GULINUX_COMMONS_CIMG_HISTOGRAM
#include <CImg.h>
namespace GuLinux {
  template<typename T>
  class Histogram {
  public:
    
    struct Bin {
      double start, middle, end;
      uint32_t count, index;
    };
    
    Histogram(const cimg_library::CImg<T> &image) : _image(image), _min{image.min_max(_max)}, _mean{image.mean()} {};
    std::vector<Bin> bins(uint32_t bins_count, int64_t min = -1, int64_t max = -1) const {
      min = min < 0 ? _min : min;
      max = max < 0 ? _max : max;
      auto hist = _image.get_histogram(bins_count, min, max);
      std::vector<Bin> bins(hist.size());
      int current_bin = 0;
      const double bin_width = (static_cast<double>(max)-static_cast<double>(min))/static_cast<double>(bins_count);
      std::transform(hist.begin(), hist.end(), bins.begin(), [&](uint32_t val){ 
	double start = bin_width*current_bin;
	return Bin{ start, start + bin_width/2., start+bin_width, val, current_bin++ };
      });
      return bins;
    }
    inline T min() const { return _min; }
    inline T max() const { return _max; }
    inline double mean() const { return _mean; }
  private:
    const cimg_library::CImg<T> _image;
    T _min, _max;
    const double _mean;
  };
}

#endif