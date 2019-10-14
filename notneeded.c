// this file contains stuff that used to be parts of the code and
// aren't anymore, but might be needed in the future


static struct Point2D intersect_lines(
	struct Point2D astart, struct Point2D aend,
	struct Point2D bstart, struct Point2D bend)
{
	// https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection#Given_two_points_on_each_line
	double
		x1 = astart.x, y1 = astart.y,
		x2 = aend.x,   y2 = aend.y,
		x3 = bstart.x, y3 = bstart.y,
		x4 = bend.x,   y4 = bend.y;

	double denom = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4);
	// TODO: handle the case where denom is very small
	return (struct Point2D){
		( (x1*y2-y1*x2)*(x3-x4) - (x1-x2)*(x3*y4-y3*x4) )/denom,
		( (x1*y2-y1*x2)*(y3-y4) - (y1-y2)*(x3*y4-y3*x4) )/denom,
	};
}
