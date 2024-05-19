#include "polygon.hpp"
#include <algorithm>
#include <delimiter.hpp>
#include <functional>
#include <iterator>
#include <numeric>
#include <scopeGuard.hpp>

namespace zak = zakozhurnikova;

std::istream& zak::operator>>(std::istream& in, Point& point)
{
  std::istream::sentry guard(in);
  if (!guard)
  {
    return in;
  }
  using del = zak::DelimiterChar;
  Point tmp{ 0, 0 };
  in >> del{ '(' } >> tmp.x >> del{ ';' } >> tmp.y >> del{ ')' };
  if (in)
  {
    point = tmp;
  }
  return in;
}

std::istream& zak::operator>>(std::istream& in, Polygon& polygon)
{
  std::istream::sentry guard(in);
  if (!guard)
  {
    return in;
  }
  std::size_t vertexCount = 0;
  in >> vertexCount;
  if (vertexCount < 3)
  {
    in.setstate(std::ios::failbit);
    return in;
  }
  std::vector< Point > temp{};
  using input_it_t = std::istream_iterator< Point >;
  std::copy_n(input_it_t{ in }, vertexCount, std::back_inserter(temp));
  if (in && temp.size() == vertexCount)
  {
    polygon.points = temp;
  }
  return in;
}

bool zak::operator==(const Point& lhs, const Point& rhs)
{
  return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

bool zak::operator<(const Point& lhs, const Point& rhs)
{
  return (lhs.x < rhs.x) && (lhs.y < rhs.y);
}

bool zak::operator<=(const Point& lhs, const Point& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

bool zak::operator>=(const Point& lhs, const Point& rhs)
{
  return !(lhs < rhs);
}

struct PolygonAreaComputer
{
  zak::Point p1;
  double operator()(double area, const zak::Point& p2, const zak::Point& p3);
};

double PolygonAreaComputer::operator()(double area, const zak::Point& p2, const zak::Point& p3)
{
  area += std::abs((p2.x - p1.x) * (p3.y - p1.y) - (p3.x - p1.x) * (p2.y - p1.y));
  p1 = p2;
  return area;
}

double zak::Polygon::getArea() const
{
  using namespace std::placeholders;
  auto accumulateArea = std::bind(PolygonAreaComputer{ points.at(1) }, _1, _2, points.at(0));
  return std::accumulate(points.cbegin(), points.cend(), 0.0, accumulateArea) / 2;
}

bool zak::operator==(const Polygon& lhs, const Polygon& rhs)
{
  if (lhs.points.size() != rhs.points.size())
  {
    return false;
  }
  return std::equal(lhs.points.cbegin(), lhs.points.cend(), rhs.points.cbegin());
}
