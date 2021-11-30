# RayTracing

## Description

This is a ray tracer written in C++. The ray tracer takes an input file that specifies multiple types of light and light source, material property and shape, and it calculates the lighting effect on each pixel of the output image based on the input file. The types of light it supports include ambient light, point light, spot light and directional light. The material properties it supports include ambient, specular, reflection and refraction. The shapes it supports include sphere, triangle with and without normal of each vertex, plane and box.

**This is a course project (Spring 2021 Semester), and the original submission page is on: https://sites.google.com/umn.edu/haoyutan/home/project3-b. However, the submission webpage is for reference only so please still use the code from GitHub. Thanks a lot!**

## How to Run
### There are two ways to run this project
1. There is an executable file **ray.exe** under the **Project3-Resources/PGA** folder. 
2. There is a **Makefile** under the **Project3-Resources/PGA** folder. Running it using **make** on command line will generate an executable file **rayTrace**.

On command line, running the executable with an input .txt file that specifies the scene setting will generate an output image. There are some sample input files in the ./SampleTest and ./test_files folder.

## Sample Files & Output Images

### My Self-define Test Files

(1) Point Light

Files:

    ./test_files/ball_pointLight.txt (left)

    ./test_files/dd1.txt (right)

![43c7d7e1f0c159aa0b3baaf13d3a0e2](https://user-images.githubusercontent.com/35856355/139576198-a88813fc-d975-4f21-9219-f94a7a024985.png)

(2) Directional Light

Files:

    ./test_files/ballDirection.txt (left)

    ./test_files/dd.txt (right)

![8924fd49a9422a022824947b035d97e](https://user-images.githubusercontent.com/35856355/139576268-d87f09c8-2b90-4fd9-9cce-58ff1b8570b6.png)

(3) Spot Light

Files:

    ./test_files/xbox.txt (left)
    
    ./test_files/ddspot.txt (right)
    
![f8f1ec64161f3438862a5b14b69d44d](https://user-images.githubusercontent.com/35856355/139576368-3d31b577-0c97-4592-878b-4f65ce3b17af.png)

(4) Mixed Lights

File:
  
    ./SampleTest/light-mix.txt

![697f25e7f8224ffe00e2725da9d0221](https://user-images.githubusercontent.com/35856355/139576422-4547a622-d30b-48ac-ad0c-1cd8e62dbc76.png)

(5) Shadow

File:
    
    ./test_files/shadow.txt
    
![92659be9c4ebda97a99c6998f83919b](https://user-images.githubusercontent.com/35856355/139576442-2ce14b19-f394-4e08-982f-56bcc34b2a90.png)

(6) Triangle

File:

    ./SampleTest/triangle1.txt
    
![d37c2191248b8f358e33b7306986a2c](https://user-images.githubusercontent.com/35856355/139576533-e0b74f65-6c61-42ec-aa89-d4a1e7805fd7.png)

(7) Normal Triangle

File:

    ./SampleTest/normal_tri.txt
    
![55673d60d7e41478582de346300b223](https://user-images.githubusercontent.com/35856355/139576597-25356f84-762a-4023-86de-5ca607389c22.png)

File:

    ./test_files/normal_tri2.txt

![efebbcfab2cab1f53cdc5c739b92cb7](https://user-images.githubusercontent.com/35856355/139576848-64f4cf37-ddd9-4d59-b623-fbbc60442e71.png)


(8) Plane

File:

    ./SampleTest/plane.txt

![6fea92d5c275b8d04068d08e75f5eee](https://user-images.githubusercontent.com/35856355/139576687-d028bf9b-1ed6-468b-9b4f-5912c0717c7d.png)

(9) Box

File:

    ./SampleTest/box-mix.txt (left)
    
    ./SampleTest/box-mix2.txt (right)
    
![564767b31ac737ea1e98e9c4d428f8e](https://user-images.githubusercontent.com/35856355/139576738-3504da86-2130-4f1c-a8dd-d8a4a13cb940.png)

(10) Ambient

File:

    ./test_files/ambient (1).txt
    
![a619176f34e84fb25e3229309dfd852](https://user-images.githubusercontent.com/35856355/139576815-ce144459-a411-4b42-b6bd-7eec0cd931b9.png)

### Sample Test Files:
![1dadf8fd1cd288976770f26dfb13f1e](https://user-images.githubusercontent.com/35856355/139577028-c38a7818-b202-4193-bd0c-2603d08bf3cd.png)

![b73265e845709c4aa6f70803db198fa](https://user-images.githubusercontent.com/35856355/139577030-d1bae543-d6b6-4b4a-a9c1-02fe192486b8.png)
![ba1159ecbd2bbc7f070e5795436ddef](https://user-images.githubusercontent.com/35856355/139577034-6433c4df-c4e0-4595-95e1-8760bc5b2703.png)
![7fbc0615e7b791469eb30be9a2ac7b8](https://user-images.githubusercontent.com/35856355/139577040-24153743-2e46-4567-b933-c03475e73b36.png)
![c9ec51eafc7905109f274aba5ed19bb](https://user-images.githubusercontent.com/35856355/139577086-d211c8e1-fc75-437f-afaa-cf086ddbf546.png)
![7aa924a741938bf5774570c3e967a5d](https://user-images.githubusercontent.com/35856355/139577089-b40e3aea-808e-4f05-a281-0f55c9dde858.png)

### Others

**(1)Colorful Planets**

![colourful](https://user-images.githubusercontent.com/35856355/139577187-a51d7eac-34f3-4c44-9076-195639071f2a.png)

**Thanks my friend Ruichen He for the design of this file**

**(2)Ugly Ink Bottle**

![fc54be838ab5255d9f902382e3f6ed2](https://user-images.githubusercontent.com/35856355/139577292-741a6b65-535b-46f8-a9a6-5dd161596764.png)


