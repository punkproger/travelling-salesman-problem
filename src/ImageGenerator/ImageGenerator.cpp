#include "ImageGenerator/ImageGenerator.hpp"

#include <algorithm>

#include "ThirdParty/Bitmap/bitmap_image.hpp"

using namespace TSP;

constexpr size_t size_x{800};
constexpr size_t size_y{800};

constexpr double scaler{1.5f};

constexpr size_t circle_radius{4};


namespace {
	double transformCoordinate(double coordinate, double scale, double offset)
	{
		return (coordinate*scale)+offset - (size_x/2)/scaler;
	}

	void printTrip(cartesian_canvas& canvas,
				   const std::vector<size_t>& path,
				   const std::vector<Point>& points,
				   double scale_x,
				   double scale_y,
				   double offset_x,
				   double offset_y)
	{

		canvas.pen_color(0, 0, 0);
		for (size_t i = 0; i < path.size()-1; ++i)
		{
			canvas.line_segment(transformCoordinate(points[path[i]].x, scale_x, -offset_x),
								transformCoordinate(points[path[i]].y, scale_y, -offset_y),
								transformCoordinate(points[path[i+1]].x, scale_x, -offset_x),
								transformCoordinate(points[path[i+1]].y, scale_y, -offset_y));
		}
	}

	void printCities(cartesian_canvas& canvas,
					 const std::vector<Point>& points,
					 double scale_x,
					 double scale_y,
					 double offset_x,
					 double offset_y)
	{
		canvas.pen_color(255, 0, 0);
		for (const auto& point : points)
		{
			canvas.fill_circle(transformCoordinate(point.x, scale_x, -offset_x), 
							   transformCoordinate(point.y, scale_y, -offset_y),
							   circle_radius);
		}
	}
}

void ImageGenerator::saveImageFromPath(const std::string& filename,
					   				   const std::vector<size_t>& path,
					   				   const std::vector<Point>& points)
{
	double min_x{0xFFFFFF}, max_x{-0xFFFFFF}, min_y{0xFFFFFF}, max_y{-0xFFFFFF};

	auto set_min_max_coordinates = 
	[&](const Point& point)
	{
		min_x = std::min(point.x, min_x);
		min_y = std::min(point.y, min_y);
		max_x = std::max(point.x, max_x);
		max_y = std::max(point.y, max_y);
	};
	std::for_each(points.begin(), points.end(), set_min_max_coordinates);

	double scale_x = (size_x/(max_x-min_x))/scaler;
	double scale_y = (size_y/(max_y-min_y))/scaler;

	double offset_x = min_x*scale_x;
	double offset_y = min_y*scale_y;

	cartesian_canvas canvas(size_x, size_y);

	printTrip(canvas, path, points, scale_x, scale_y, offset_x, offset_y);
	printCities(canvas, points, scale_x, scale_y, offset_x, offset_y);

	canvas.image().save_image(filename);
}