#ifndef MY_CLASS_H
#define MY_CLASS_H


//Ray
//need fixed!!!!!!
class Shape;
struct Ray {
	Point3D start = Point3D();
	Line3D rayLine = Line3D();
	Dir3D dir = Dir3D();
	std::vector<Shape*> shapes;

	Ray(Point3D p, Line3D rl) {
		start = p;
		rayLine = rl.normalized();
		//printf("In Generating rayline\n");
		//rayLine.print();
		dir = rayLine.dir();
	}

	Ray() {}

};

//need fixed!!!!!!!!!
struct HitInformation {
	Point3D hitPoint;
	Dir3D normal;
	//Material m;
	Shape* shape;
	float t = 0;

	HitInformation() {}

	/*
	HitInformation(Point3D p, Dir3D norm, Material mat) {
		hitPoint = p;
		normal = norm;
		m = mat;
	}
	*/

	HitInformation(Point3D p, Dir3D norm, Shape* s, float t_val) {
		hitPoint = p;
		normal = norm;
		shape = s;
		t = t_val;
	}
};




//Abstract Base Class
class Shape {
public:
	Material m;
	//type of shape
	//0: sphere
	//1: triangle
	//2: normal_triangle
	int type;

	Shape() {};
	Shape(Material mat, int t):m(mat), type(t) {};

	virtual void printShape() = 0;

	virtual bool Compare(Shape* shape) = 0;

	virtual ~Shape(){};
};

//sphere
class Sphere: public Shape {
public:
	Point3D pos; //default?
	float r;  //default?
	

	Sphere():Shape(){}
	
	Sphere(Material m):Shape(m, 0){
		pos = Point3D();
		float r = 1;
	}

	Sphere(Point3D p, float radius, Material m):Shape(m, 0) {
		pos = p;
		r = radius;
	}

	//same: return true
	//different: return false
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	virtual bool Compare(Shape* shape){
		if (shape->type != 0) {
			//printf("sphere: type does not match\n");
			return false;
		}
		else {
			Sphere* sphere1 = (Sphere*)shape;
			if (pos.x != sphere1->pos.x) return false;
			if (pos.y != sphere1->pos.y) return false;
			if (pos.z != sphere1->pos.z) return false;
			if (r != sphere1->r) return false;
		}
		return true;
	}

	//virtual bool compare(Shape s1) {
	//	return false;
	//}

	virtual void printShape() {
		printf("I'm sphere, pos %f, %f, %f, radius %f\n", pos.x, pos.y, pos.z, r);
	}

	virtual ~Sphere() {}
};


//triangle
class Triangle: public Shape {
public:
	Vertex v1;
	Vertex v2;
	Vertex v3;

	Triangle():Shape(){}
	Triangle(Material m):Shape(m, 1){

	}

	Triangle(Vertex vert1, Vertex vert2, Vertex vert3, Material m):Shape(m, 1){
		v1 = vert1;
		v2 = vert2;
		v3 = vert3;
		
	}

	//box triangle
	Triangle(Vertex vert1, Vertex vert2, Vertex vert3, Material m, int t) :Shape(m, t) {
		v1 = vert1;
		v2 = vert2;
		v3 = vert3;

	}

	virtual bool Compare(Shape* shape) {
		//printf("my type is: %i\n", type);
		if (shape->type != type) { 
			//printf("triangle: type not match\n");
			return false; 
		}
		else {
			Triangle* t1 = (Triangle*)shape;
			if (v1.x != t1->v1.x) return false;
			if (v1.y != t1->v1.y) return false;
			if (v1.z != t1->v1.z) return false;
			if (v2.x != t1->v2.x) return false;
			if (v2.y != t1->v2.y) return false;
			if (v2.z != t1->v2.z) return false;
			if (v3.x != t1->v3.x) return false;
			if (v3.y != t1->v3.y) return false;
			if (v3.z != t1->v3.z) return false;
		}
		return true;
	}

	virtual void printShape() {
		printf("I'm triangle, v1: %f, %f, %f, v2: %f, %f, %f, v3: %f, %f, %f\n", v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
	}

	Dir3D computeNormal(Ray r) {
		Dir3D norm1 = cross(v3.v - v1.v, v2.v - v1.v).normalized();
		Dir3D norm2 = cross(v2.v - v1.v, v3.v - v1.v).normalized();
		Dir3D rayDir = (-1.0) * r.dir.normalized();

		return dot(rayDir, norm1) > 0 ? norm1 : norm2;
	}

	virtual ~Triangle() {}
};

class NormalTriangle : public Shape {
public:
	Vertex v1;
	Vertex v2;
	Vertex v3;
	Normal n1;
	Normal n2;
	Normal n3;

	NormalTriangle(Vertex vert1, Vertex vert2, Vertex vert3, Normal norm1,
		Normal norm2, Normal norm3, Material m):Shape(m, 2) {
		v1 = vert1;
		v2 = vert2;
		v3 = vert3;
		n1 = norm1;
		n2 = norm2;
		n3 = norm3;
	}

	virtual bool Compare(Shape* shape) {
		if (shape->type != 2) {
			//printf("normal triangle: type does not match\n");
			return false;
		}
		else {
			NormalTriangle* nt1 = (NormalTriangle*)shape;
			if (v1.x != nt1->v1.x) return false;
			if (v1.y != nt1->v1.y) return false;
			if (v1.z != nt1->v1.z) return false;

			if (v2.x != nt1->v2.x) return false;
			if (v2.y != nt1->v2.y) return false;
			if (v2.z != nt1->v2.z) return false;

			if (v3.x != nt1->v3.x) return false;
			if (v3.y != nt1->v3.y) return false;
			if (v3.z != nt1->v3.z) return false;

			if (n1.x != nt1->n1.x) return false;
			if (n1.y != nt1->n1.y) return false;
			if (n1.z != nt1->n1.z) return false;

			if (n2.x != nt1->n2.x) return false;
			if (n2.y != nt1->n2.y) return false;
			if (n2.z != nt1->n2.z) return false;

			if (n3.x != nt1->n3.x) return false;
			if (n3.y != nt1->n3.y) return false;
			if (n3.z != nt1->n3.z) return false;
		}
		return true;
	}

	virtual void printShape() {
		printf("I'm normal triangle, v1: %f, %f, %f, v2: %f, %f, %f, v3: %f, %f, %f\n", v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
	}

	Dir3D computeNormal(float w1, float w2, float w3) {
		//interpolate the normal
		Dir3D dir = w1 * n1.n + w2 * n2.n + w3 * n3.n;
		return dir.normalized();
	}

	virtual ~NormalTriangle() {}
};

class Plane:public Shape {
public:
	Point3D pos;
	Dir3D norm;
	Plane(Point3D position, Dir3D normal, Material mat):Shape(mat, 3){
		pos = position;
		norm = normal;
	}

	virtual void printShape() {
		printf("I'm a plane, pos: %f, %f, %f\n", pos.x, pos.y, pos.z);
	}

	virtual bool Compare(Shape* shape) {
		if (shape->type != 3) {
			//printf("normal triangle: type does not match\n");
			return false;
		}
		else {
			Plane* p = (Plane*)shape;
			if (pos.x != p->pos.x) return false;
			if (pos.y != p->pos.y) return false;
			if (pos.z != p->pos.z) return false;
			if (norm.x != p->norm.x) return false;
			if (norm.y != p->norm.y) return false;
			if (norm.z != p->norm.z) return false;

			//material
		}
		return true;
	}
	virtual ~Plane() {}
};

//https://lousodrome.net/blog/light/2017/01/03/intersection-of-a-ray-and-a-cone/
class Cone :public Shape {
public:
	float half_angle;
	Dir3D down;
	Point3D top;
	float depth;

	Cone(Point3D t, Dir3D d, float h_ang, float dpt, Material mat):Shape(mat, 5){
		top = t;
		down = d;
		half_angle = h_ang;
		depth = dpt;
	}

	virtual void printShape() {
		printf("I'm a cone, top: %f, %f, %f, dir: %f, %f, %f\n", top.x, top.y, top.z,
			down.x, down.y, down.z);
	}

	virtual bool Compare(Shape* shape) {
		if (shape->type != 5) {
			//printf("normal triangle: type does not match\n");
			return false;
		}
		else {
			Cone* cone = (Cone*)shape;
			if (top.x != cone->top.x) return false;
			if (top.y != cone->top.y) return false;
			if (top.z != cone->top.z) return false;

			if (down.x != cone->down.x) return false;
			if (down.y != cone->down.y) return false;
			if (down.z != cone->down.z) return false;

			if (half_angle != cone->half_angle) return false;
			//material
		}
		return true;
	}

	virtual ~Cone(){}
};

/*
class Box :public Shape {
public:
	Point3D center;
	Dir3D size;

	Point3D p1;
	Point3D p2;
	Point3D p3;
	Point3D p4;
	Point3D p5;
	Point3D p6;
	Point3D p7;
	Point3D p8;


	Box(Point3D c, Dir3D s, Material mat):Shape(mat, 4){
		center = c;
		size = s;

		float half_x = s.x / 2.0;
		float half_y = s.y / 2.0;
		float half_z = s.z / 2.0;

		p1 = Point3D(c.x - half_x, c.y + half_y, c.z - half_z);
		p2 = Point3D(c.x - half_x, c.y - half_y, c.z - half_z);
		p3 = Point3D(c.x + half_x, c.y - half_y, c.z - half_z);
		p4 = Point3D(c.x + half_x, c.y + half_y, c.z - half_z);
		p5 = Point3D(c.x - half_x, c.y + half_y, c.z + half_z);
		p6 = Point3D(c.x - half_x, c.y - half_y, c.z + half_z);
		p7 = Point3D(c.x + half_x, c.y - half_y, c.z + half_z);
		p8 = Point3D(c.x + half_x, c.y + half_y, c.z + half_z);
	}

	virtual void printShape() {
		printf("I'm a box, center: %f, %f, %f, size: %f, %f, %f\n", center.x, center.y, center.z,
			size.x, size.y, size.z);
	}

	virtual bool Compare(Shape* shape) {
		if (shape->type != 4) {
			//printf("normal triangle: type does not match\n");
			return false;
		}
		else {
			Box* b = (Box*)shape;
			if (center.x != b->center.x) return false;
			if (center.y != b->center.y) return false;
			if (center.z != b->center.z) return false;
			if (size.x != b->size.x) return false;
			if (size.y != b->size.y) return false;
			if (size.z != b->size.z) return false;

			//material
		}
		return true;
	}
	virtual ~Box() {}


};
*/
/*
class Plane:public Shape{
public:
	Vertex v1;
	Vertex v2;
	Vertex v3;
	Vertex v4;
	Plane3D plane1;
	Plane3D plane2;
	bool singlePlane; // 0: not single plane, 1: single plane

	Plane(Vertex vert1, Vertex vert2, Vertex vert3, Vertex vert4, Material m):Shape(m, 3){
		v1 = vert1;
		v2 = vert2;
		v3 = vert3;
		v4 = vert4;
		singlePlane = false;
		Plane3D planeBase = vee(v1.v, v2.v, v3.v);
		Plane3D planeABD = vee(v1.v, v2.v, v4.v);
		Plane3D planeBCD = vee(v2.v, v3.v, v4.v);
		Plane3D planeCAD = vee(v3.v, v1.v, v4.v);
		if (dot(planeABD, planeBase) < 0) {
			//v4 is on the different side of the v3.
			//v1,v2,v3 form plane1 and v1, v2,v4 form plane2
			plane1 = planeBase;
			plane2 = planeABD;
			printf("case 1\n");
		}
		else if (dot(planeBCD, planeBase) < 0) {
			//v4 is on different side of v1
			//v1,v2,v3 form plane1 and v2,v3,v4 form plane2
			plane1 = planeBase;
			plane2 = planeBCD;
			printf("case 2\n");
		}
		else if (dot(planeCAD, planeBase) < 0) {
			//v4 is on different side of v2
			//v1,v2, v3 form plane1 and v3,v1,v4 form plane2
			plane1 = planeBase;
			plane2 = planeCAD;
			printf("case 3\n");
		}
		else {
			//one plane
			plane1 = planeBase;
			//remeber to handle!!!!
			singlePlane = true;
			printf("case 4\n");
		}
	}

	void printShape() {
		printf("I'm a Plane, v1: %f, %f, %f, v2: %f, %f, %f, v3: %f, %f, %f, v4: %f, %f, %f\n", v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z, v4.x, v4.y, v4.z);
	}

	virtual bool Compare(Shape* shape) {
		if (shape->type != 3) {
			//printf("normal triangle: type does not match\n");
			return false;
		}
		else {
			Plane* p = (Plane*)shape;
			if (v1.x != p->v1.x) return false;
			if (v1.y != p->v1.y) return false;
			if (v1.z != p->v1.z) return false;

			if (v2.x != p->v2.x) return false;
			if (v2.y != p->v2.y) return false;
			if (v2.z != p->v2.z) return false;

			if (v3.x != p->v3.x) return false;
			if (v3.y != p->v3.y) return false;
			if (v3.z != p->v3.z) return false;

			if (v4.x != p->v4.x) return false;
			if (v4.y != p->v4.y) return false;
			if (v4.z != p->v4.z) return false;
		}
		return true;
	}

	Dir3D computNormal() {
		return Dir3D(0.0, 0.0, 0.0);
	}

	virtual ~Plane() {}
};
*/
#endif
