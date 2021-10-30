//Set the global scene parameter variables
//TODO: Set the scene parameters based on the values in the scene file

#ifndef PARSE_PGA_H
#define PARSE_PGA_H

#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>
//MYFILE


//Camera & Scene Parmaters (Global Variables)
//Here we set default values, override them in parseSceneFile()

//Image Parmaters
int img_width = 640, img_height = 480;
//int img_width = 600, img_height = 800;
std::string imgName = "raytraced.bmp";

//Camera Parmaters
Point3D eye = Point3D(0, 0, 0);
Dir3D forward = Dir3D(0, 0, -1).normalized();
Dir3D up = Dir3D(0, 1, 0).normalized();
Dir3D right = Dir3D(-1, 0, 0).normalized();
float halfAngleVFOV = 45;
//float halfAngleVFOV = 35;


//Current Materials
//only record current material until the next one is read
//Default: a matte white surface
//for each channel, e.g. red, ar, dr, sr, tr act as Ka, Kd, Ks, Kt
Color ambient = Color(0, 0, 0); //ambient color (ar, ag, ab)
Color diffuse = Color(1, 1, 1); //diffuse color (dr, dg, db)
Color specular = Color(0, 0, 0); //highlight and reflected ray (sr,sg,sb)
float specular_pow = 5.0; // ns
Color transmissive = Color(0, 0, 0); // transmissive color (tr, tg, tb)
float ior = 0.0; //index of refrection

//Background Color 
Color bg_color = Color(0.0, 0.0, 0.0);



//ambient light
//factor actually
Color ambient_light = Color(0, 0, 0);

//recursion depth limit
int max_depth = 5;

//vectors
/*
std::vector<DirectionalLight> d_lights;
std::vector<PointLight> p_lights;
std::vector<SpotLight> s_lights;
*/
std::vector<Light> lights;
std::vector<Shape*> shapes;

float threshold = 0.001;

//for Project3B only
int num_vert = -1;
int num_norm = -1;
int curr_vert = 0;
int curr_norm = 0;
Vertex* vertices;
Normal* normals;

int jitter_supersampling = 0; //0 no sampling, 1 sampling
int camera = 0; //0 no sampling, 1 sampling
float focus = 0.0;

void parseSceneFile(std::string fileName) {
	//TODO: Override the default values with new data from the file "fileName"
	  //reading the file

	//int sphereCount = 0;
	//int materialCount = 0;

	std::string line;
	std::ifstream input(fileName.c_str());

	// check for errors in opening the file
	if (input.fail()) {
		std::cout << "Can't open file '" << fileName << "'" << std::endl;
		return;
	}

	std::string command;
	while (input >> command) {

		if (command[0] == '#') {
			getline(input, line);
			std::cout << "Skipping comment: " << command << line << std::endl;
		}

		//std::cout << command << std::endl;

		if (command == "sphere:") {
			//printf("I'm in sphere!!!\n");
			float x = 0.0;
			float y = 0.0;
			float z = 0.0;
			float radius = 0.0;
			input >> x >> y >> z >> radius;
			Material mat = Material(ambient, diffuse, specular, specular_pow, transmissive, ior);
			//Sphere s = Sphere();
			//s.pos = Point3D(x, y, z);

			//if (radius > 0)
				//s.r = radius;
			//s.m = mat;

			if (radius <= 0)
				radius = 1;
			
			//add to vector
			shapes.push_back(new Sphere(Point3D(x, y, z), radius, mat));

			//sphereCount += 1;

		}
		else if (command == "camera_pos:") {
			printf("in camera\n");
			float x = eye.x;
			float y = eye.y;
			float z = eye.z;
			input >> x >> y >> z;
			eye = Point3D(x, y, z);

		}
		else if (command == "camera_fwd:") {
			printf("in camera forward\n");
			float fx, fy, fz;
			input >> fx >> fy >> fz;

			forward = Dir3D(fx, fy, fz);
			forward.print();
			forward = forward.normalized();

		}
		else if (command == "camera_up:") {
			printf("in camera up\n");
			float ux, uy, uz;
			input >> ux >> uy >> uz;


			up = Dir3D(ux, uy, uz);
			up.print();
			up = up.normalized();
		}
		else if (command == "camera_fov_ha:") {
			printf("in camera fov\n");
			float fov;
			input >> fov;
			if (fov > 0)
				halfAngleVFOV = fov;
		}
		else if (command == "film_resolution:") {
			printf("in film resolution\n");
			int w = img_width;
			int h = img_height;
			//std::cout << "command is: " << command << std::endl;
			input >> w >> h;
			if (w > 0 && h > 0) {
				img_width = w;
				img_height = h;
			}
		}
		else if (command == "output_image:") {
			printf("in output image\n");
			std::string img_name = imgName;
			input >> img_name;
			imgName = img_name;
		}
		else if (command == "background:") {
			printf("in background\n");
			float r, g, b;

			input >> r >> g >> b;

			if (r > 0.0) {
				bg_color.r = r;
			}
			if (g > 0.0) {
				bg_color.g = g;
				
			}
			if (b > 0.0) {
				bg_color.b = b;
			}
		}
		else if (command == "material:") {
			printf("in material\n");
			float ar = 0.0, ag = 0.0, ab = 0.0;
			float dr = 0.0, dg = 0.0, db = 0.0;
			float sr = 0.0, sg = 0.0, sb = 0.0;
			float ns = 0.0;
			float tr = 0.0, tg = 0, tb = 0;
			float ior_in;

			input >> ar >> ag >> ab;
			input >> dr >> dg >> db;
			input >> sr >> sg >> sb;
			input >> ns;
			input >> tr >> tg >> tb;
			input >> ior_in;

			printf("%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n",
				ar, ag, ab, dr, dg, db, sr, sg, sb, ns, tr, tg, tb, ior_in);


			ambient = Color(ar, ag, ab);

			diffuse = Color(dr, dg, db);

			specular = Color(sr, sg, sb);
			specular_pow = ns;


			transmissive = Color(tr, tg, tb);
			ior = ior_in;

			//printf("================in reading material: ==================\n");
			//printf("ambient: %f, %f, %f\n", ambient.r, ambient.g, ambient.b);
			//printf("diffuse: %f, %f, %f\n", diffuse.r, diffuse.g, diffuse.b);
			//printf("specular: %f, %f, %f\n", specular.r, specular.g, specular.b);
			//printf("specular power: %f\n", specular_pow);
			//printf("transimissive: %f, %f, %f\n", transmissive.r, transmissive.g, transmissive.b);
			//printf("index of refrection: %f\n", ior);
			//printf("=======================================================\n");
			//materialCount++;

		}

		else if (command == "directional_light:") {
			printf("in directional light\n");
			float r, g, b;
			float x, y, z;

			input >> r >> g >> b;
			input >> x >> y >> z;

			Light d_light = Light();
			Color light = Color(0.0, 0.0, 0.0);
			if (r >= 0.0 && g >= 0.0 && b >= 0.0)
				light = Color(r, g, b);
			Dir3D dir = Dir3D(x, y, z);
			d_light.SetDirectionalLight(light, dir);
			lights.push_back(d_light);

			//printf("light: %f, %f, %f\n", light.r, light.g, light.b);
			//printf("directional light: %f, %f, %f, %f, %f, %f\n", d_light.light.r,
				//d_light.light.g, d_light.light.b, d_light.dir.x, d_light.dir.y,
				//d_light.dir.z);
		}

		else if (command == "point_light:") {
			printf("in point light\n");
			float r, g, b;
			float x, y, z;

			input >> r >> g >> b;
			input >> x >> y >> z;

			Light p_light = Light();
			Color light = Color(0.0, 0.0, 0.0);
			if (r >= 0.0 && g >= 0.0 && b >= 0.0)
				light = Color(r, g, b);
			Point3D pos = Point3D(x, y, z);
			p_light.SetPointLight(light, pos);
			lights.push_back(p_light);

			//printf("point light: %f, %f, %f, %f, %f, %f\n", p_light.light.r,
			//	p_light.light.g, p_light.light.b, p_light.pos.x, p_light.pos.y,
			//	p_light.pos.z);
		}

		else if (command == "spot_light:") {
			printf("in spot light\n");
			float r, g, b;
			float px, py, pz;
			float dx, dy, dz;
			float a1, a2;

			input >> r >> g >> b;
			input >> px >> py >> pz;
			input >> dx >> dy >> dz;
			input >> a1 >> a2;

			Light s_light = Light();
			Color light = Color(0.0, 0.0, 0.0);
			if (r >= 0.0 && g >= 0.0 && b >= 0.0)
				light = Color(r, g, b);
			Point3D pos = Point3D(px, py, pz);
			Dir3D dir = Dir3D(dx, dy, dz);
			s_light.SetSpotLight(light, pos, dir, a1, a2);
			lights.push_back(s_light);

			//printf("spot light: %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n",
			//	s_light.light.r,
			//	s_light.light.g, s_light.light.b, s_light.pos.x,
			//	s_light.pos.y, s_light.pos.z, s_light.dir.x, s_light.dir.y,
			//	s_light.dir.z);
		}

		else if (command == "ambient_light:") {
			printf("in ambient light\n");
			float r, g, b;

			input >> r >> g >> b;
			if (r >= 0.0 && g >= 0.0 && b >= 0.0)
				ambient_light = Color(r, g, b);
		}

		else if (command == "max_depth:") {
			printf("in max depth\n");
			int n = 0;
			input >> n;
			max_depth = n;
		}

		else if (command == "max_vertices:") {
			int n = 0;
			input >> n;
			if (n < 0) {
				n = 0;
			}
			num_vert = n;
			curr_vert = 0;
			vertices = new Vertex[num_vert];
		}
		else if (command == "max_normals:") {
			int n = 0;
			input >> n;
			if (n < 0) n = 0;
			num_norm = n;
			curr_norm = 0;
			normals = new Normal[num_norm];
		}
		else if (command == "vertex:") {
			if(num_vert < 0) {
				printf("need to set the number of vertex before assigning value to it\n");
				//exit the program
				exit(EXIT_FAILURE);
			}
			if (curr_vert < num_vert) {
				//add to array
				float x = 0.0, y = 0.0, z = 0.0;
				input >> x >> y >> z;
				vertices[curr_vert] = Vertex(x, y, z);
				curr_vert += 1;
			}
			else {
				//delete after done!!!!
				printf("verties array is full\n");
			}
		}
		else if(command == "normal:") {
			if (num_norm < 0) {
				printf("need to set the number of normal before assigning value to it\n");
				//exit the program
				exit(EXIT_FAILURE);
			}
			if (curr_norm < num_norm) {
				float x = 0.0, y = 0.0, z = 0.0;
				input >> x >> y >> z;
				normals[curr_norm] = Normal(x, y, z);
				curr_norm += 1;
			}
			else {
				printf("normal array is full\n");
			}

		}
		else if(command == "triangle:") {
			int v1 = 0, v2 = 0, v3 = 0;
			input >> v1 >> v2 >> v3;
			if (v1 < num_vert && v2 < num_vert && v3 < num_vert) {
				Material mat = Material(ambient, diffuse, specular, specular_pow, transmissive, ior);
				shapes.push_back(new Triangle(vertices[v1], vertices[v2], vertices[v3], mat));
				//shapes.back()->printShape();
			}
			else {
				printf("index of at least one of the vertices is out of bound, this triangle is ignored\n");
			}

		}
		else if(command == "normal_triangle:") {
			int v1 = 0, v2 = 0, v3 = 0;
			int n1 = 0, n2 = 0, n3 = 0;
			input >> v1 >> v2 >> v3;
			input >> n1 >> n2 >> n3;
			if (v1 < num_vert && v2 < num_vert && v3 < num_vert
				&& n1 < num_norm && n2 < num_norm && n3 < num_norm) {
				Material mat = Material(ambient, diffuse, specular, specular_pow, transmissive, ior);
				shapes.push_back(new NormalTriangle(vertices[v1], vertices[v2], vertices[v3],
									normals[n1], normals[n2], normals[n3], mat));
			}
			else {
				printf("index or normal of at least one of the vertices is out of bound, this triangle is ignored\n");
			}
			
		}
		else if (command == "plane:") {
			float x = 0.0, y = 0.0, z = 0.0;
			float nx = 0.0, ny = 0.0, nz = 0.0;

			input >> x >> y >> z;
			input >> nx >> ny >> nz;

			Point3D p = Point3D(x, y, z);
			Dir3D d = Dir3D(nx, ny, nz);

			Material mat = Material(ambient, diffuse, specular, specular_pow, transmissive, ior);
			//printf("ambient: %f, %f, %f\n", mat.a.r, mat.a.g, mat.a.b);
			//printf("diffuse: %f, %f, %f\n", mat.d.r, mat.d.g, mat.d.b);
			//printf("specular: %f, %f, %f\n", mat.s.r, mat.s.g, mat.s.b);
			//printf("specular power: %f\n", mat.s_pow);
			//printf("transimissive: %f, %f, %f\n", mat.t.r, mat.t.g, mat.t.b);
			//printf("index of refrection: %f\n", mat.ior);
			shapes.push_back(new Plane(p, d, mat));
		}
		/*
		else if (command == "box:") {
			float x = 0.0, y = 0.0, z = 0.0;
			float sx = 0.0, sy = 0.0, sz = 0.0;

			input >> x >> y >> z;
			input >> sx >> sy >> sz;

			Point3D center = Point3D(x, y, z);
			Dir3D size = Dir3D(sx, sy, sz);

			Material mat = Material(ambient, diffuse, specular, specular_pow, transmissive, ior);
			shapes.push_back(new Box(center, size, mat));
		}
		*/

		else if (command == "box:") {
			float x = 0.0, y = 0.0, z = 0.0;
			float sx = 0.0, sy = 0.0, sz = 0.0;

			input >> x >> y >> z;
			input >> sx >> sy >> sz;

			Point3D center = Point3D(x, y, z);

			

			float half_x = sx / 2.0;
			float half_y = sy / 2.0;
			float half_z = sz / 2.0;

			Vertex p1 = Vertex(center.x - half_x, center.y + half_y, center.z - half_z);
			Vertex p2 = Vertex(center.x - half_x, center.y - half_y, center.z - half_z);
			Vertex p3 = Vertex(center.x + half_x, center.y - half_y, center.z - half_z);
			Vertex p4 = Vertex(center.x + half_x, center.y + half_y, center.z - half_z);
			Vertex p5 = Vertex(center.x - half_x, center.y + half_y, center.z + half_z);
			Vertex p6 = Vertex(center.x - half_x, center.y - half_y, center.z + half_z);
			Vertex p7 = Vertex(center.x + half_x, center.y - half_y, center.z + half_z);
			Vertex p8 = Vertex(center.x + half_x, center.y + half_y, center.z + half_z);

			Material mat = Material(ambient, diffuse, specular, specular_pow, transmissive, ior);

			//front
			shapes.push_back(new Triangle(p1, p2, p3, mat, 4));
			shapes.push_back(new Triangle(p3, p4, p1, mat,4));

			//side right
			shapes.push_back(new Triangle(p4, p3, p7, mat, 4));
			shapes.push_back(new Triangle(p7, p8, p4, mat, 4));

			//back
			shapes.push_back(new Triangle(p8, p7, p6, mat, 4));
			shapes.push_back(new Triangle(p6, p5, p8, mat, 4));

			//side left
			shapes.push_back(new Triangle(p5, p6, p2, mat, 4));
			shapes.push_back(new Triangle(p2, p1, p5, mat, 4));

			//top
			shapes.push_back(new Triangle(p5, p1, p4, mat, 4));
			shapes.push_back(new Triangle(p4, p8, p5, mat, 4));

			//bottom
			shapes.push_back(new Triangle(p6, p2, p3, mat, 4));
			shapes.push_back(new Triangle(p3, p7, p6, mat, 4));
			
		}
		else if (command == "cone:") {
			float tx = 0.0, ty = 0.0, tz = 0.0;
			float dx = 0.0, dy = 0.0, dz = 0.0;
			float half_angle = 0.0;
			float depth = 0.0;

			input >> tx >> ty >> tz;
			input >> dx >> dy >> dz;
			input >> half_angle;
			input >> depth;

			Point3D top = Point3D(tx, ty, tz);
			Dir3D down = Dir3D(dx, dy, dz);

			Material mat = Material(ambient, diffuse, specular, specular_pow, transmissive, ior);
			shapes.push_back(new Cone(top, down, half_angle,depth, mat));


		}
		else if (command == "jitter_supersampling:") {
			int sample = 0;
			input >> sample;
			jitter_supersampling = sample;
			printf("supersampling? %i\n", jitter_supersampling);
		}
		else if (command == "lens_simulation:") {
			int c = 0;
			float f = 0.0;

			input >> c;
			input >> f;

			camera = c;
			focus = f;

			printf("camera? %i, focus: %f\n", camera, focus);
		}
	}

	//find the right-ward vector
	right = cross(up, forward);

	if (right.magnitude() == 0) {
		printf("up and forward is parallel, using the default value instead");
		forward = Dir3D(0, 0, -1).normalized();
		up = Dir3D(0, 1, 0).normalized();
		right = cross(up, forward);

	}

	right = right.normalized();
	std::cout << "final right is: " << right.x << ", " << right.y << ", " << right.z << std::endl;

	//right = cross(up, forward).normalized();
	//std::cout << "normalized right dir is: " << right << std::endl;

	//up = up.normalized();
	//forward = cross(right,up);
	forward = forward.normalized();

	up = cross(forward, right);
	up = up.normalized();



	//using namespace std;
	std::cout << "=====================\n";
	std::cout << "image width: " << img_width << ", image height: " << img_height << std::endl;
	std::cout << "image name: " << imgName << std::endl;

	std::cout << "Camera Information:\n";
	std::cout << "position: " << eye.x << ", " << eye.y << ", " << eye.z << std::endl;
	std::cout << "forward: " << forward.x << ", " << forward.y << ", " << forward.z << std::endl;
	std::cout << "up: " << up.x << ", " << up.y << ", " << up.z << std::endl;
	std::cout << "right is: " << right.x << ", " << right.y << ", " << right.z << std::endl;
	std::cout << "half angle FOV: " << halfAngleVFOV << std::endl;

	/*
	std::cout << "Sphere Information:\n";
	std::cout << "sphere position: " << spherePos.x << ", " << spherePos.y << ", " << spherePos.z << std::endl;
	std::cout << "sphere radius: " << sphereRadius << std::endl;
	*/

	std::cout << "Background Information:\n";
	std::cout << "background color: " << bg_color.r << ", " << bg_color.g << ", " << bg_color.b << std::endl;

	/*
	std::cout << "Material Information:\n";
	std::cout << "ambient color is: " << ambient.r << ", " << ambient.g << ", " << ambient.b << std::endl;
	std::cout << "diffuse color is: " << diffuse.r << ", " << diffuse.g << ", " << diffuse.b << std::endl;
	std::cout << "specular color is: " << specular.r << ", " << specular.g << ", " << specular.g << std::endl;
	std::cout << "specular power: " << specular_pow << std::endl;
	std::cout << "transmissive color is: " << transmissive.r << ", " << transmissive.g << ", " << transmissive.b << std::endl;
	std::cout << "index of refrection is: " << ior << std::endl;
	*/

	std::cout << "Lighting Information\n";


	for (int i = 0; i < lights.size(); i++) {
		if (lights[i].type == 1) {
			printf("\n");
			printf("directional light %i\n", i);
			printf("light color: %f, %f, %f\n", lights[i].light.r,
				lights[i].light.g, lights[i].light.b);
			printf("light direction: %f, %f, %f\n", lights[i].dir.x,
				lights[i].dir.y, lights[i].dir.z);
		}
		else if (lights[i].type == 0) {
			printf("\n");
			printf("point light %i\n", i);
			printf("light color: %f, %f, %f\n", lights[i].light.r,
				lights[i].light.g, lights[i].light.b);
			printf("light position: %f, %f, %f\n", lights[i].pos.x,
				lights[i].pos.y, lights[i].pos.z);
		}
		else if (lights[i].type == 2) {
			printf("spot light %i\n", i);
			printf("light color: %f, %f, % f\n", lights[i].light.r,
				lights[i].light.g, lights[i].light.b);
			printf("light direction: %f, %f, %f\n", lights[i].dir.x,
				lights[i].dir.y, lights[i].dir.z);
			printf("light position: %f, %f, %f\n", lights[i].pos.x,
				lights[i].pos.y, lights[i].pos.z);
			printf("angle 1: %f, angle 2: %f\n", lights[i].a1, lights[i].a2);
		}
	}


	std::cout << "Ambient Information:\n";
	std::cout << "light color: " << ambient_light.r << ", " << ambient_light.g << ", " << ambient_light.b << std::endl;

	std::cout << "Others:\n";
	std::cout << "max_depth: " << max_depth << std::endl;

	printf("num of vertices: %i\n", num_vert);
	printf("num of normals: %i\n", num_norm);
	printf("current vertices: %i\n", curr_vert);
	printf("current normal: %i\n", curr_norm);

	/*
	printf("Sphere Information:\n");

	for (int i = 0; i < shapes.size(); i++) {
		if (shapes[i]->type == 0) {
			printf("sphere: %i\n", i);
			Sphere* s = (Sphere*)shapes[i];
			printf("pos: %f, %f, %f\n", s->pos.x, s->pos.y, s->pos.z);
			printf("radius: %f\n", s->r);
			printf("ambient: %f, %f, %f\n", s->m.a.r, s->m.a.g, s->m.a.b);
			printf("diffuse: %f, %f, %f\n", s->m.d.r, s->m.d.g, s->m.d.b);
			printf("specular: %f, %f, %f\n", s->m.s.r, s->m.s.g, s->m.s.b);
			printf("specular power: %f\n", s->m.s_pow);
			printf("transimissive: %f, %f, %f\n", s->m.t.r, s->m.t.g, s->m.t.b);
			printf("index of refrection: %f\n", s->m.ior);
		}
		
		else if (shapes[i]->type == 1 || shapes[i]->type == 4) {
			printf("triangle: %i\n", i);
			Triangle* tri = (Triangle*)shapes[i];
			//tri->Compare(tri);
			printf("v1: %f, %f, %f\n", tri->v1.x, tri->v1.y, tri->v1.z);
			printf("v2: %f, %f, %f\n", tri->v2.x, tri->v2.y, tri->v2.z);
			printf("v3: %f, %f, %f\n", tri->v3.x, tri->v3.y, tri->v3.z);
			printf("ambient: %f, %f, %f\n", tri->m.a.r, tri->m.a.g, tri->m.a.b);
			printf("diffuse: %f, %f, %f\n", tri->m.d.r, tri->m.d.g, tri->m.d.b);
			printf("specular: %f, %f, %f\n", tri->m.s.r, tri->m.s.g, tri->m.s.b);
			printf("specular power: %f\n", tri->m.s_pow);
			printf("transimissive: %f, %f, %f\n", tri->m.t.r, tri->m.t.g, tri->m.t.b);
			printf("index of refrection: %f\n", tri->m.ior);
		}
		
		else if (shapes[i]->type == 3) {
			Plane* p = (Plane*)shapes[i];
			printf("plane: %i\n", i);
			p->pos.print();
			p->norm.print();
			//p->Compare(p);
			printf("ambient: %f, %f, %f\n", p->m.a.r, p->m.a.g, p->m.a.b);
			printf("diffuse: %f, %f, %f\n", p->m.d.r, p->m.d.g, p->m.d.b);
			printf("specular: %f, %f, %f\n", p->m.s.r, p->m.s.g, p->m.s.b);
			printf("specular power: %f\n", p->m.s_pow);
			printf("transimissive: %f, %f, %f\n", p->m.t.r, p->m.t.g, p->m.t.b);
			printf("index of refrection: %f\n", p->m.ior);
		}
		else if (shapes[i]->type == 5) {
			Cone* c = (Cone*)shapes[i];
			printf("cone: %i\n", i);
			c->top.print();
			c->down.print();
			//p->Compare(p);
			printf("ambient: %f, %f, %f\n", c->m.a.r, c->m.a.g, c->m.a.b);
			printf("diffuse: %f, %f, %f\n", c->m.d.r, c->m.d.g, c->m.d.b);
			printf("specular: %f, %f, %f\n", c->m.s.r, c->m.s.g, c->m.s.b);
			printf("specular power: %f\n", c->m.s_pow);
			printf("transimissive: %f, %f, %f\n", c->m.t.r, c->m.t.g, c->m.t.b);
			printf("index of refrection: %f\n", c->m.ior);
		}
	}
	*/
	std::cout << "=====================\n";


}
#endif