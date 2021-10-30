#ifndef MY_STRUCT_H
#define MY_STRUCT_H

#include <vector>


struct Light {
	Color light;
	int type = -1;
	// 0 point light
	// 1 directional light
	// 2 spot light
	Dir3D dir; // Directional, spot
	Point3D pos; // PointLight, spot
	float a1 = 0.0; // spot
	float a2 = 0.0; // spot

	Light() {
		light = Color();
		dir = Dir3D();
		pos = Point3D();
	};

	void SetPointLight(Color l, Point3D position) {
		type = 0; // this is point light type
		light = l;
		pos = position;
	}

	void SetDirectionalLight(Color l, Dir3D direction) {
		type = 1; // this is directional light
		light = l;
		dir = direction;
	}

	void SetSpotLight(Color l, Point3D position, Dir3D direction,
		float angle_1, float angle_2) {
		type = 2; // this is spot light
		light = l;
		pos = position;
		dir = direction;
		a1 = angle_1;
		a2 = angle_2;

	}

};

struct Material {
	Color a; //ambient color (ar, ag, ab)
	Color d; //diffuse color (dr, dg, db)
	Color s; //highlight and reflected ray (sr,sg,sb)
	float s_pow; // ns
	Color t; // transmissive color (tr, tg, tb)
	float ior; //index of refrection

	Material() {};
	Material(Color ab, Color dif, Color spe, int ns, Color tran, float index) {
		a = ab;
		d = dif;
		s = spe;
		s_pow = ns;
		t = tran;
		ior = index;
	};

	bool compare(Material m) {
		if (!a.compare(m.a)) return false;
		if (!d.compare(m.d)) return false;
		if (!s.compare(m.s)) return false;
		if (!t.compare(m.t)) return false;
		if (s_pow != m.s_pow) return false;
		if (ior != m.ior) return false;
		return true;
	}

};

/*
struct Sphere {
	Point3D pos; //default?
	float r;  //default?
	Material m; //Material Information
	
	//create a sphere with default pos, r, and material
	Sphere() {
		pos = Point3D();
		float r = 1;
		m = Material();
	}

	//same: return true
	//different: return false
	bool compare(Sphere s1) {
		if (pos.x != s1.pos.x) return false;
		if (pos.y != s1.pos.y) return false;
		if (pos.z != s1.pos.z) return false;
		if (r != s1.r) return false;
		//if (!m.compare(s1.m)) return false;
		return true;
	}

};
*/

struct Vertex {
	float x;
	float y;
	float z;
	Point3D v;

	Vertex(float x1=0, float y1=0, float z1=0): x(x1), y(y1), z(z1){
		v = Point3D(x1, y1, z1);
	}
};

struct Normal {
	float x;
	float y;
	float z;
	Dir3D n;

	Normal(float x1=0, float y1=0, float z1=0): x(x1), y(y1), z(z1) {
		n = Dir3D(x1, y1, z1);
	}
};


#endif