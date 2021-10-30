//CSCI 5607 HW3 - Rays & Files
//This HW has three steps:
// 1. Compile and run the program (the program takes a single command line argument)
// 2. Understand the code in this file (rayTrace_pga.cpp), in particular be sure to understand:
//     -How ray-sphere intersection works
//     -How the rays are being generated
//     -The pipeline from rays, to intersection, to pixel color
//    After you finish this step, and understand the math, take the HW quiz on canvas
// 3. Update the file parse_pga.h so that the function parseSceneFile() reads the passed in file
//     and sets the relevant global variables for the rest of the code to product to correct image

//To Compile: g++ -fsanitize=address -std=c++11 rayTrace_pga.cpp

//For Visual Studios
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES 
#endif

#define _USE_MATH_DEFINES

#include <cmath>
//Images Lib includes:
#define STB_IMAGE_IMPLEMENTATION //only place once in one .cpp file
#define STB_IMAGE_WRITE_IMPLEMENTATION //only place once in one .cpp files
#include "../include/image_lib.h" //Defines an image class and a color class

#include <stdlib.h>

//#3D PGA
#include "../include/PGA_3D.h"
#include "../include/print_PGA.h"

//High resolution timer
#include <chrono>

//include my own defined structure
//must include before parse_pga.h
#include "../include/MyStruct.h"
#include "../include/MyClass.h"

//Scene file parser
#include "../include/parse_pga.h"

bool Refract(Ray ray, HitInformation hit, Ray* glass);
Ray Reflect(Ray ray, HitInformation hit);
Color ApplyLightingModel(Ray ray, HitInformation hit, int depth);
Color Ambient(HitInformation hit);
Color DiffuseContribution(Light l, HitInformation hit);
Color SpecularContribution(Light l, Ray r, HitInformation hit);

/*
bool raySphereIntersect(Point3D rayStart, Line3D rayLine, Point3D sphereCenter, float sphereRadius) {
    Point3D projPoint = dot(rayLine, sphereCenter) * rayLine;      //Project to find closest point between circle center and line [proj(sphereCenter,rayLine);]
    float distSqr = projPoint.distToSqr(sphereCenter);          //Point-line distance (squared)
    float d2 = distSqr / (sphereRadius * sphereRadius);             //If distance is larger than radius, then...
    if (d2 > 1) return false;                                   //... the ray missed the sphere
    float w = sphereRadius * sqrt(1 - d2);                          //Pythagorean theorem to determine dist between proj point and intersection points
    Point3D p1 = projPoint - rayLine.dir() * w;                   //Add/subtract above distance to find hit points
    Point3D p2 = projPoint + rayLine.dir() * w;

    if (dot((p1 - rayStart), rayLine.dir()) >= 0) return true;     //Is the first point in same direction as the ray line?
    if (dot((p2 - rayStart), rayLine.dir()) >= 0) return true;     //Is the second point in same direction as the ray line?
    return false;
}

bool raySphereIntersect_fast(Point3D rayStart, Line3D rayLine, Point3D sphereCenter, float sphereRadius){
  Dir3D dir = rayLine.dir();
  float a = dot(dir,dir);
  Dir3D toStart = (rayStart - sphereCenter);
  float b = 2 * dot(dir,toStart);
  float c = dot(toStart,toStart) - sphereRadius*sphereRadius;
  float discr = b*b - 4*a*c;
  if (discr < 0) return false;
  else{
    float t0 = (-b + sqrt(discr))/(2*a);
    float t1 = (-b - sqrt(discr))/(2*a);
    if (t0 > 0 || t1 > 0) return true;
  }
  return false;
}
*/


bool FindIntersection(Ray ray, HitInformation *hit) {
    //printf("in find intersection\n");
    Dir3D dir = ray.dir;
    float a = dot(dir, dir);
    float min_dist = std::numeric_limits<float>::infinity();
    bool hitted = false;

    

    for (int i = 0; i < shapes.size(); i++) {
        //printf("in check each sphere in scene\n");
        if (shapes[i]->type == 0) {
            Sphere* sphere = (Sphere*)shapes[i];
            Dir3D toStart = (ray.start - sphere->pos);
            float b = 2 * dot(dir, toStart);
            float c = dot(toStart, toStart) - sphere->r * sphere->r;
            float discr = b * b - 4 * a * c;
            if (discr < 0) continue;

            float t0 = (-b - sqrt(discr)) / (2 * a);
            float t1 = (-b + sqrt(discr)) / (2 * a);

            //printf("t0: %f, t1: %f\n", t0, t1);
            if (t0 < threshold) {
                //use t1
            //printf("t0 less than threshold\n");
                if (t1 >= threshold) {
                    if (t1 < min_dist) {
                        //printf("using t1\n");
                        min_dist = t1;
                        //save information to hit
                        Point3D hit_p = ray.start + t1 * dir;
                        Dir3D nor = (sphere->pos - hit_p).normalized();
                        hit->hitPoint = hit_p;
                        hit->normal = nor;
                        hit->shape = sphere;
                        hit->t = t1;
                        hitted = true;
                    }
                }
            }
            else {
                //use t0
            //printf("t0 greater than threshold\n");
                if (t0 < min_dist) {
                    //printf("using t0\n");
                    min_dist = t0;
                    //save information to hit
                    Point3D hit_p = ray.start + t0 * dir;
                    Dir3D normal = hit_p - sphere->pos;
                    hit->hitPoint = hit_p;
                    hit->normal = normal.normalized();
                    hit->shape = sphere;
                    hit->t = t0;
                    hitted = true;
                }
            }
        }
        else if (shapes[i]->type == 1 || shapes[i]->type == 4) {
            //ray triangle intersect
            Triangle* tri = (Triangle*)shapes[i];
            Point3D p1 = tri->v1.v;
            Point3D p2 = tri->v2.v;
            Point3D p3 = tri->v3.v;
            Plane3D triPlane = vee(p1, p2, p3);

            HomogeneousPoint3D hHit = wedge(ray.rayLine.normalized(), triPlane);
            //ray.rayLine.normalized().print();
            //triPlane.print();
            //hHit.print();
            if (hHit.w == 0) {
                //parallel
                continue;
            }
            Point3D hitPoint = Point3D(hHit);
            //hitPoint.print();

            //calculate the barycentric coordinate
            float a = vee(tri->v2.v, tri->v3.v, hitPoint).magnitude() / triPlane.magnitude(); //v1's coordinate
            float b = vee(tri->v3.v, tri->v1.v, hitPoint).magnitude() / triPlane.magnitude(); //v2's coordinate
            float c = vee(tri->v1.v, tri->v2.v, hitPoint).magnitude() / triPlane.magnitude(); // v3's coordinate

            if (a <= 1.00 && b <= 1.00 && c <= 1.00 && a+b+c <= 1.001) {
                //hitted
                //direction check!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                //Line3D l1 = vee(ray.start, hitPoint);
                //if (dot(l1, ray.rayLine) > 0) {
                Dir3D d1 = (hitPoint - ray.start).normalized();
                if(dot(d1,ray.dir) > 0){
                    //calculate the dist from hitpoint to ray
                    float d = (hitPoint - ray.start).magnitude();
                    //compare with minimal
                    if (d >= threshold && d < min_dist) {
                        Dir3D hit_norm = tri->computeNormal(ray).normalized(); //already normalized in function
                        hit->hitPoint = hitPoint;
                        hit->normal = hit_norm;
                        hit->shape = tri;
                        hit->t = d;
                        hitted = true;
                        min_dist = d;
                    }
                }
            }

        }
        else if (shapes[i]->type == 2){
            //ray normal triangle intersect
            NormalTriangle* ntri = (NormalTriangle*)shapes[i];
            Plane3D triPlane = vee(ntri->v1.v, ntri->v2.v, ntri->v3.v);

            HomogeneousPoint3D hHit = wedge(ray.rayLine, triPlane);
            if (hHit.w == 0) {
                //parallel
                continue;
            }
            Point3D hitPoint = Point3D(hHit);

            //calculate the barycentric coordinate
            float a = vee(ntri->v2.v, ntri->v3.v, hitPoint).magnitude() / triPlane.magnitude(); //v1's coordinate
            float b = vee(ntri->v3.v, ntri->v1.v, hitPoint).magnitude() / triPlane.magnitude(); //v2's coordinate
            float c = vee(ntri->v1.v, ntri->v2.v, hitPoint).magnitude() / triPlane.magnitude(); // v3's coordinate

            if (a <= 1.00 && b <= 1.00 && c <= 1.00 && a + b + c <= 1.001) {
                //hitted
                //direction check!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                //Line3D l1 = vee(ray.start, hitPoint);
                Dir3D d1 = (hitPoint - ray.start).normalized();



                if (dot(d1, ray.dir) > 0 ) {
                    //calculate the dist from hitpoint to ray
                    float d = (hitPoint - ray.start).magnitude();
                    //compare with minimal
                    
                    if (d >= threshold && d < min_dist) {
                        Dir3D hit_norm = ntri->computeNormal(a, b, c).normalized(); //already normalized in function
                        //Dir3D hit_norm = ntri->computeNormal(a, b, c).normalized(); //already normalized in function
                        if (dot(hit_norm, ray.dir) > 0) {
                            hit_norm = (-1.0 * hit_norm).normalized();
                        }
                        hit->hitPoint = hitPoint;
                        hit->normal = hit_norm;
                        hit->shape = ntri;
                        hit->t = d;
                        hitted = true;
                        min_dist = d;
                    }
                }
            }
        }
        else if (shapes[i]->type == 3) {
            //plane
            Plane* plane = (Plane*)shapes[i];
            Dir3D norm = plane->norm;
            norm = norm.normalized();
            Point3D pos = plane->pos;
            
            float w = (-1.0) * (norm.x * pos.x + norm.y * pos.y + norm.z * pos.z);
            Plane3D p = Plane3D(norm.x, norm.y, norm.z, w);

            HomogeneousPoint3D hHit = wedge(ray.rayLine, p);
            if (hHit.w == 0) {
                //parallel
                continue;
            }
            Point3D hitPoint = Point3D(hHit);
            
            Dir3D d1 = (hitPoint - ray.start).normalized();
            if (dot(d1, ray.dir) > 0) {
                float d = (hitPoint - ray.start).magnitude();

                if (d >= threshold && d < min_dist) {
                    if (dot(norm, ray.dir) > 0) {
                        norm = (-1.0 * norm).normalized();
                    }

                    hit->hitPoint = hitPoint;
                    hit->normal = norm;
                    hit->shape = plane;
                    hit->t = d;

                    hitted = true;
                    min_dist = d;
                }
            }
        }
        else if (shapes[i]->type == 5) {
            //cones
            Cone* cone = (Cone*)shapes[i];

            Dir3D d = (ray.dir).normalized();
            Dir3D v = (cone->down).normalized();
            float ha = cone->half_angle;
            float dotDV = dot(d,v);
            Dir3D CO = ray.dir - cone->top;

            float a = pow(dotDV, 2.0) - (cosf(ha))* (cosf(ha));
            float b = 2.0*(dot(dotDV * CO, v) - dot(d, CO)*pow(cosf(ha), 2.0));
            float c = pow(dot(CO, v), 2.0) - dot(CO, CO)*pow(cosf(ha), 2.0);

            //same as sphere
            float discr = b * b - 4 * a * c;
            if (discr < 0.0) continue;

            float t0 = (-b - sqrt(discr)) / (2.0 * a);
            float t1 = (-b + sqrt(discr)) / (2.0 * a);
            // here we need to do some test
            Point3D hit_p = ray.start + t0 * ray.dir;
            float dist = (cone->top - hit_p).magnitude();
            if (dist > cone->depth) {
                continue;
            }


            if (t0 < threshold) {
                if (t1 >= threshold) {
                    if (t1 < min_dist) {
                        min_dist = t1;
                        hitted = true;
                        Point3D hit_p = ray.start + t1 * ray.dir;
                        Dir3D tth = (hit_p - cone->top).normalized();
                        
                        Dir3D norm = tth - 1 / cosf(ha) * ((cone->down).normalized());
                        norm = norm.normalized();

                        if (dot(norm, ray.dir) > 0) {
                            norm = (-1.0 * norm).normalized();
                        }
                        
                        

                        hit->hitPoint = hit_p;
                        hit->normal = norm;
                        hit->shape = cone;
                        hit->t = t1;
                    }
                }
            }
            else {
                if (t0 < min_dist) {
                    min_dist = t0;
                    hitted = true;

                    Dir3D norm = (cone->down) * sinf(ha);
                    norm = norm.normalized();

                    if (dot(norm, ray.dir) > 0) {
                        norm = (-1.0 * norm).normalized();
                    }

                    Point3D hit_p = ray.start + t0 * ray.dir;

                    hit->hitPoint = hit_p;
                    hit->normal = norm;
                    hit->shape = cone;
                    hit->t = t0;
                }
            }
        }
        
    }

    return hitted;
}

Color EvaluateRayTree(Ray ray, int depth) {
    bool hit_sth = false;
    HitInformation hit;
    //printf("ray start: %f, %f, %f\n", ray.start.x, ray.start.y, ray.start.z);
    //printf("ray dir: %f, %f, %f\n", ray.dir.x, ray.dir.y, ray.dir.z);
    //printf("ray line: %f, %f, %f\n", ray.rayLine.x, ray.rayLine.y, ray.rayLine.z);

    //NEED TO FIX!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //split to a main find intersection, then seperate to different shape's intersection
    hit_sth = FindIntersection(ray, &hit);
    

    if (hit_sth) {
        return ApplyLightingModel(ray, hit, depth);
    }
    else {
        return bg_color;
    }
    
    //should not reach here
    return Color();
}

Color ApplyLightingModel(Ray ray, HitInformation hit, int depth) {
    Color contribution = Color(0, 0, 0);

    //Direct light contribution
    for (int i = 0; i < lights.size(); i++) {
        Light l = lights.at(i);

        Dir3D rayDir = Dir3D();
        float light_to_hit = std::numeric_limits<float>::infinity();

        //point light & spot light
        if (l.type == 0 || l.type == 2) {
            rayDir = l.pos - hit.hitPoint;
            light_to_hit = rayDir.magnitude();
        }
        else if (l.type == 1) {
            //directional light
            rayDir = (-1.0) * l.dir;
        }

        Line3D line = vee(hit.hitPoint, rayDir).normalized();

        //shadow => check if light is blocked
        Ray shadow = Ray(hit.hitPoint, line);
        HitInformation shadow_hit;
        bool blocked = false;

        blocked = FindIntersection(shadow, &shadow_hit);

        //shadow might have something to do with this
        if (blocked && shadow_hit.t < light_to_hit) {
            //blokced
            continue;
        }

        //diffuse and specular

        contribution = contribution + DiffuseContribution(l, hit);
        contribution = contribution + SpecularContribution(l, ray, hit);
    }
    
    
    //add reflect
    //printf("Depth is %i \n", depth);
    
    if (depth < max_depth) {
        //printf("in reflection: depth: %i\n", depth);
        
        
        Ray mirror = Reflect(ray, hit);
        Color Kr = hit.shape->m.s;
        contribution = contribution + Kr * EvaluateRayTree(mirror, depth + 1);
        
        
        
        Ray glass = Ray();
        bool refr = Refract(ray, hit, &glass);

        if (refr) {
            //printf("depth: %i, depth\n", depth);
            Color Kt = hit.shape->m.t;
            contribution = contribution + Kt * EvaluateRayTree(glass, depth + 1);
        }
        
    }
    
    
    //printf("I am here!!!!!!!!!!!");
    contribution = contribution +  Ambient(hit);
    //printf("final color is: %f, %f, %f\n", contribution.r, contribution.g, contribution.b);
    return contribution;
}




bool Refract(Ray ray, HitInformation hit, Ray* glass) {
    //printf("====in refract====\n");
    float old_ior = 1.0;
    float new_ior = hit.shape->m.ior;
    int size = ray.shapes.size();
    if (size > 0) {
        Shape* outest = ray.shapes.back();
        //printf("outest shape type is:%i\n", outest->type);
        old_ior = outest->m.ior;

        Shape* hit_sphere = hit.shape;
        //printf("hit shape type is:%i\n", hit_sphere->type);
        if (hit_sphere->Compare(outest)) {
            //printf("same1\n");

            int index = ray.shapes.size() - 2;
            if (index >= 0) {
                new_ior = (ray.shapes.at(index))->m.ior;
            }
            else {
                new_ior = 1.0;
            }
        }
        
       
    }
    
    float ng = old_ior / new_ior;
    Dir3D d = ray.dir;
    Dir3D norm = hit.normal;
    float dp = dot(d, norm);

    float sqr = 1.0 - ng * ng * (1.0 - dp * dp);

    if (sqr < 0) return false;

    Dir3D t = ng * (d - dp * norm) - sqrt(sqr) * norm;
    Line3D line = vee(hit.hitPoint, t).normalized();

    Ray r = Ray(hit.hitPoint, line);

    std::vector<Shape*> k = ray.shapes; 
    //printf("size of k is: %i\n", k.size());
    
    if (k.size() == 0) {
        
        if (hit.shape->type == 0 || hit.shape->type == 4) {
            k.push_back(hit.shape);
        }
        
        //k.push_back(hit.shape);
    }
    else if (k.size() > 0) {
        bool same = false;
        for (int i = k.size() - 1; i >= 0; i--) { //check
            Shape* current = k.at(i);
            if (current->Compare(hit.shape)) {
                //printf("i is: %i\n", i);
                k.erase(k.begin() + i); //check
                //printf("erase!\n");
                same = true;
                break;
            }

        }
        if(!same) {
            
            if (hit.shape->type == 0 || hit.shape->type == 4) {
                k.push_back(hit.shape);
            }
            
            //k.push_back(hit.shape);
        }
        
       // printf("I'm gonna out of loop\n");
        /*for (int i = 0; i < k.size(); i++) {
            Sphere s = k.at(i);
            //printf("sphere is: %f, %f, %f, %f\n", s.pos.x, s.pos.y, s.pos.z, s.r);
        }*/
    }
    //printf("after setting, size of k is: %i\n", k.size());
    //printf("==================\n");
    r.shapes = k;
    *glass = r;
    return true;
}

//r = 2* dot(n, v)*n - v
Ray Reflect(Ray ray, HitInformation hit) {
    //printf("====in reflect====\n");

    Dir3D n = hit.normal;
    Dir3D rayDir = ray.dir;
    rayDir = ((-1.0) * rayDir).normalized();
    Dir3D r = 2.0 * dot(n, rayDir) * n - rayDir;

    Point3D hitPoint = hit.hitPoint;
    Line3D rLine = vee(hitPoint, r).normalized();

    Ray m_ray = Ray(hitPoint, rLine);
    m_ray.shapes = ray.shapes; //delete if no refraction!!!!
    //printf("print content in sphere\n");
    /*for (int i = 0; i < m_ray.spheres.size(); i++) {
        Sphere s = m_ray.spheres[i];
        //printf("sphere is: %f, %f, %f, %f\n", s.pos.x, s.pos.y, s.pos.z, s.r);
    }*/
    //printf("==================\n");
    return m_ray;
}

//TODO
//Ld = kd*I*max(0, dot(n, l))
//kd:diffuse coefficient(dr, dg, db for each channel)
//I: illumination from source
Color DiffuseContribution(Light l, HitInformation hit) {
    if (l.type == 0 || l.type == 2) {
        //calculate theta
        Dir3D light_l = (l.pos - hit.hitPoint).normalized();
        float theta = dot(hit.normal, light_l);
        theta = (0 < theta)?theta:0;

        Color I = Color(0.0,0.0,0.0);
        //calculate I
        if (l.type == 0) {
            //point light
            float r_sqr = (l.pos - hit.hitPoint).magnitudeSqr();
            r_sqr = 1 / r_sqr;
            I = l.light * r_sqr;
        }
        else{
            //printf("in spot light\n");
            //spot light
            //calculate the spot light area angle
            Dir3D spot_dir = (l.dir).normalized();
            Dir3D light_dir = (hit.hitPoint - l.pos).normalized();
            //spot_dir.print();
            float cos_angle = dot(spot_dir, light_dir);
            //printf("cos_angle: %f\n", cos_angle);
            float spot_angle = acosf(cos_angle) * (180.0/M_PI);
            //printf("spot_angle: %f, cos_angle is %f\n", spot_angle,cos_angle);

            if (spot_angle <= l.a2) {
                //printf("============================\n");
                //printf("a1 = %f\n", l.a1);
                //printf("a2 = %f\n", l.a2);
                //printf("spot_angle: %f, inside a1\n", spot_angle);
                float r_sqr = (l.pos - hit.hitPoint).magnitudeSqr();
                r_sqr = 1 / r_sqr;
                I = l.light * r_sqr;
                
                //printf("spot_angle:%f\n", spot_angle);
                if (spot_angle >= l.a1) {
                    //printf("outside a1\n");
                    //between a1 and a2, falloff
                    
                    float ratio = 1 - (spot_angle - l.a1) / (l.a2 - l.a1);
                    //printf("Spot angle is %f, ratio is %f, rsqr is %f\n", spot_angle, ratio,r_sqr);
                    I= I * ratio;
                }
                
            }
            //else just pure black

            
        }

        Color kd = hit.shape->m.d;

        return kd * I * theta;
        
    }
    else if (l.type == 1) {
        Dir3D light_l = ((-1.0) * (l.dir)).normalized();
        float theta = dot(hit.normal, light_l);
        theta = (0 < theta) ? theta : 0;

        Color I = l.light;
        Color kd = hit.shape->m.d;

        return kd * I * theta;
    }

    //should not reach here
    return Color(0, 0, 0);
}

//r = 2(dot(n, v)n - v)
Color SpecularContribution(Light l, Ray r, HitInformation hit) {
    if (l.type == 0 || l.type == 2) {

        //calculate dot(V,R)^n
        Dir3D norm = hit.normal;

        Dir3D light_l = (l.pos - hit.hitPoint).normalized();
        Dir3D R = 2 * dot(norm, light_l) * norm - light_l;
        Dir3D v_norm = ((-1.0) * r.dir).normalized();
        //printf("\n");
        //norm.print();
        //l_norm.print();
        float alpha = dot(v_norm, R);
        if (alpha < 0) alpha = 0;
        //printf("cos_alpha is: %f\n", alpha);

        float spow = (float)(hit.shape->m.s_pow);
        float highlight = pow(alpha, spow);
        //printf("highlight is %f\n", highlight);
        
        Color I = Color(0.0, 0.0, 0.0);
        //calculate illumination
        if (l.type == 0) {
            float r_sqr = (l.pos - hit.hitPoint).magnitudeSqr();
            r_sqr = 1 /r_sqr;
            I = l.light * r_sqr;
        }
        else {
            Dir3D spot_dir = (l.dir).normalized();
            Dir3D light_dir = (hit.hitPoint - l.pos).normalized();
            //spot_dir.print();
            float cos_angle = dot(spot_dir, light_dir);
            //printf("cos_angle: %f\n", cos_angle);
            float spot_angle = acosf(cos_angle) * (180.0 / M_PI);
            //printf("spot_angle: %f, cos_angle is %f\n", spot_angle,cos_angle);

            if (spot_angle <= l.a2) {
                //printf("============================\n");
                //printf("a1 = %f\n", l.a1);
                //printf("a2 = %f\n", l.a2);
                //printf("spot_angle: %f, inside a1\n", spot_angle);
                float r_sqr = (l.pos - hit.hitPoint).magnitudeSqr();
                r_sqr = 1 / r_sqr;
                I = l.light * r_sqr;

                //printf("spot_angle:%f\n", spot_angle);
                if (spot_angle >= l.a1) {
                    //printf("outside a1\n");
                    //between a1 and a2, falloff

                    float ratio = 1 - (spot_angle - l.a1) / (l.a2 - l.a1);
                    //printf("Spot angle is %f, ratio is %f, rsqr is %f\n", spot_angle, ratio,r_sqr);
                    I = I * ratio;
                }

            }
        }

        //calculate the final color
        Color Ks = hit.shape->m.s;
        Color Is = Ks * highlight;
        Is = Is * I;

        return Is;
    }
    else if (l.type == 1) {
        Dir3D norm = hit.normal;

        Dir3D light_l = ((-1.0) * (l.dir)).normalized();
        Dir3D R = 2.0 * dot(norm, light_l) * norm - light_l;
        Dir3D v_norm = ((-1.0) * r.dir).normalized();
        //printf("\n");
        //norm.print();
        //l_norm.print();
        float alpha = dot(v_norm, R);
        //printf("ALPHA IS %f\n", alpha);
        //printf("cos_alpha is: %f\n", alpha);
        if (alpha < 0) alpha = 0;

        float spow = (float)(hit.shape->m.s_pow);
        float highlight = pow(alpha, spow);
        //printf("highlight is %f\n", highlight);

        //calculate illumination
        Color I = l.light;
        //printf("Intensity IS %f, %f, %f \n", I.r, I.g, I.b);
        //calculate the final color
        Color Ks = hit.shape->m.s;

        Color Is = Ks * highlight;
        Is = Is * I;
        /*
        if (Ks.r >= 0.8) {
            printf("ALPHA IS %f\n", alpha);
            printf("spow IS %f\n", spow);
            printf("Ks IS %f, %f, %f \n", Ks.r, Ks.g, Ks.b);
            printf("highlight is %f\n", highlight);
            printf("SPECULAR FOR DIRECTIONAL LIGHT IS %f, %f, %f \n", Is.r, Is.g, Is.b);
        }
        //;
        */
        return Is;
    }

    //should never reach here
    return Color(0, 0, 0);
}

Color Ambient(HitInformation hit) {
    Color a_color = hit.shape->m.a;
    Color ambient_color = a_color * ambient_light;
    
    return ambient_color;
}


/*
int main(int argc, char** argv){
  
  //Read command line paramaters to get scene file
  if (argc != 2){
     std::cout << "Usage: ./a.out scenefile\n";
     return(0);
  }
  std::string secenFileName = argv[1];

  //Parse Scene File
  parseSceneFile(secenFileName);

  float imgW = img_width, imgH = img_height;
  float halfW = imgW/2, halfH = imgH/2;
  float d = halfH / tanf(halfAngleVFOV * (M_PI / 180.0f));

  Image outputImg = Image(img_width,img_height);
  auto t_start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < img_width; i++){
    for (int j = 0; j < img_height; j++){
      //TODO: In what way does this assumes the basis is orthonormal?
      float u = (halfW - (imgW)*((i+0.5)/imgW));
      float v = (halfH - (imgH)*((j+0.5)/imgH));
      Point3D p = eye - d*forward + u*right + v*up;
      Dir3D rayDir = (p - eye); 
      Line3D rayLine = vee(eye,rayDir).normalized();  //Normalizing here is optional

      Ray ray = Ray(eye, rayLine);
      Color color = EvaluateRayTree(ray, 0);


      outputImg.setPixel(i,j, color);
      //outputImg.setPixel(i,j, Color(fabs(i/imgW),fabs(j/imgH),fabs(0))); //TODO: Try this, what is it visualizing?
    }
  }
  auto t_end = std::chrono::high_resolution_clock::now();
  printf("Rendering took %.2f ms\n",std::chrono::duration<double, std::milli>(t_end-t_start).count());

  outputImg.write(imgName.c_str());

  //free the space!!!!
  for (int i = 0; i < shapes.size(); i++) {
      delete shapes[i];
  }
  delete[] vertices;
  delete[] normals;
  return 0;
}
*/

int main(int argc, char** argv) {

    //Read command line paramaters to get scene file
    if (argc != 2) {
        std::cout << "Usage: ./a.out scenefile\n";
        return(0);
    }
    std::string secenFileName = argv[1];

    //Parse Scene File
    parseSceneFile(secenFileName);

    float imgW = img_width, imgH = img_height;
    float halfW = imgW / 2, halfH = imgH / 2;
    float d = halfH / tanf(halfAngleVFOV * (M_PI / 180.0f));

    Image outputImg = Image(img_width, img_height);
    auto t_start = std::chrono::high_resolution_clock::now();
    
    if (camera == 0 && jitter_supersampling <= 1) {
        #pragma omp parallel for
        for (int i = 0; i < img_width; i++) {
            for (int j = 0; j < img_height; j++) {
                //TODO: In what way does this assumes the basis is orthonormal?
                float u = (halfW - (imgW) * ((i + 0.5) / imgW));
                float v = (halfH - (imgH) * ((j + 0.5) / imgH));
                Point3D p = eye - d * forward + u * right + v * up;
                Dir3D rayDir = (p - eye);
                Line3D rayLine = vee(eye, rayDir).normalized();  //Normalizing here is optional


                /*
                //hard code, need to fix!!!!!!
                Sphere sphere = spheres.at(0);
                bool hit = raySphereIntersect(eye,rayLine,sphere.pos,sphere.r);

                Color color;
                if (hit) color = Color(1,1,1);
                else color = Color(0,0,0);
                */

                Ray ray = Ray(eye, rayLine);
                Color color = EvaluateRayTree(ray, 0);


                outputImg.setPixel(i, j, color);
                //outputImg.setPixel(i,j, Color(fabs(i/imgW),fabs(j/imgH),fabs(0))); //TODO: Try this, what is it visualizing?
            }
        }
    }
    else if (jitter_supersampling > 1) {
        printf("Doing jitter supersampling with %i random variables\n", jitter_supersampling);
        for (int i = 0; i < img_width; i++) {
            for (int j = 0; j < img_height; j++) {
                Color color = Color(0.0, 0.0, 0.0);
                float total_weight = 0.0;
                for (int num = 0; num < jitter_supersampling; num++) {
                    // first we do some random between 0 and 1
                    srand(time(0));
                    float nums[10];
                    for (int ind = 0; ind < 10; ind++) {
                        float num = (rand() % 1001) / 1000.0;
                        nums[ind] = num;
                        //printf("num is %f, ", num);
                    }
                    int index = rand() % 10;
                    float rv1 = nums[index];
                    for (int ind = 0; ind < 10; ind++) {
                        float num = (rand() % 1001) / 1000.0;
                        nums[ind] = num;
                        //printf("num is %f, ", num);
                    }
                    index = rand() % 10;
                    float rv2 = nums[index];

                    float u = (halfW - (imgW) * ((i + rv1) / imgW));
                    float v = (halfH - (imgH) * ((j + rv2) / imgH));

                    float dtoc = sqrt((rv1 - 0.5) * (rv1 - 0.5) + (rv2 - 0.5) * (rv2 - 0.5));// distance to center
                    float weight = 1 / (sqrt(2 * M_PI)) * exp(-dtoc / (2.0));

                    // now we need to calculate the image position
                    Point3D p = eye + d * forward + u * right + v * up;
                    //p.print();
                    Dir3D rayDir = (eye - p);
                    Line3D rayLine = vee(eye, rayDir).normalized();  //Normalizing here is optional

                    Ray ray = Ray(eye, rayLine);
                    color = color + EvaluateRayTree(ray, 0) * weight;
                    total_weight = total_weight + weight;

                }
                color = color * (1 / total_weight);
                outputImg.setPixel(img_width - 1 - i, img_height - 1 - j, color);
            }
        }

    }
    else if (camera == 1) {
        // firstly we need to compute the focus point
        // we need to recalculate the d

        printf("I am in camera mode\n");
        Point3D focusP = eye - (d / halfH) * forward;
        printf("d is %f \n", d / halfH);
        for (int i = 0; i < img_width; i++) {
            for (int j = 0; j < img_height; j++) {
                //TODO: In what way does this assumes the basis is orthonormal?
                // now we need to do the sampling to get the depth
                Color color = Color(0.0, 0.0, 0.0);
                int sampling = 20;
                for (int num = 0; num < sampling; num++) {
                    srand(time(0));
                    float nums[10];
                    for (int ind = 0; ind < 10; ind++) {
                        float num = (rand() % 1001) / 1000.0;
                        nums[ind] = num;
                        //printf("num is %f, ", num);
                    }
                    int index = rand() % 10;
                    float rv1 = nums[index];
                    for (int ind = 0; ind < 10; ind++) {
                        float num = (rand() % 1001) / 1000.0;
                        nums[ind] = num;
                        //printf("num is %f, ", num);
                    }
                    index = rand() % 10;
                    float rv2 = nums[index];
                    float scale_portion = imgW / imgH;
                    float u = ((halfW - (imgW) * ((i + rv1) / imgW)) / imgW * 2.0 * scale_portion);
                    float v = ((halfH - (imgH) * ((j + rv2) / imgH)) / imgH * 2.0);
                    // now we need to calculate the image position
                    Point3D p = eye + u * right + v * up; // set up screen
                    //p.print();
                    Dir3D rayDir = (focusP - p);
                    Line3D rayLine = vee(focusP, rayDir).normalized();  //Normalizing here is optional

                    Ray ray = Ray(focusP, rayLine);
                    color = color + EvaluateRayTree(ray, 0) * (1.0 / float(sampling));
                }
                outputImg.setPixel(img_width - 1 - i, img_height - 1 - j, color);
            }
        }

    }

    auto t_end = std::chrono::high_resolution_clock::now();
    printf("Rendering took %.2f ms\n", std::chrono::duration<double, std::milli>(t_end - t_start).count());

    outputImg.write(imgName.c_str());

    //free the space!!!!
    for (int i = 0; i < shapes.size(); i++) {
        delete shapes[i];
    }
    delete[] vertices;
    delete[] normals;
    return 0;
}
