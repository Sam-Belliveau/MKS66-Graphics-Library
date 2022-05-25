/*========== my_main.c ==========

  This is the only file you need to modify in order
  to get a working mdl project (for now).

  my_main.c will serve as the interpreter for mdl.
  When an mdl script goes through a lexer and parser,
  the resulting operations will be in the array op[].

  Your job is to go through each entry in op and perform
  the required action from the list below: {

  push: push a new origin matrix onto the origin stack

  pop: remove the top matrix on the origin stack

  move/scale/rotate: create a transformation matrix
                     based on the provided values, then
                     multiply the current top of the
                     origins stack by it.

  box/sphere/torus: create a solid object based on the
                    provided values. Store that in a
                    temporary matrix, multiply it by the
                    current top of the origins stack, then
                    call draw_polygons.

  line: create a line based on the provided values. Store
        that in a temporary matrix, multiply it by the
        current top of the origins stack, then call draw_lines.

  save: call save_extension with the provided filename

  display: view the screen
  =========================*/

#include <iostream>

#include "./legacy/parser.h"
#include "./legacy/symtab.h"
#include "./legacy/y.tab.h"
#include "./legacy/matrix.h"

#include "Engine.hpp"

using namespace SPGL;

constexpr static Float kSegments = 1024.0;

void my_main() {

    Engine engine(500, 500);

    print_symtab();
    for (int i = 0; i < lastop; i++) {

        struct command& command = op[i];

        std::cout << i << ": ";

        switch(command.opcode)
        {
        case POP: {
            engine.pop_transform();
            } break;

        case PUSH: {
            engine.push_transform(engine.get_transform());
            } break;

        case LINE: {
            engine.set_material(command.op.line.constants);

            Vec3d a(command.op.line.p0);
            Vec3d b(command.op.line.p0);
            engine.draw_line(a, b);
            } break;

        case BOX: {
            engine.set_material(command.op.box.constants);

            Vec3d a(command.op.box.d0);
            Vec3d b(command.op.box.d1);
            b = a + b * Vec3d(+1, -1, -1);
            
            Vec3d b1(a.x, a.y, a.z), b2(b.x, a.y, a.z), b3(b.x, a.y, b.z), b4(a.x, a.y, b.z);
            Vec3d t1(a.x, b.y, a.z), t2(b.x, b.y, a.z), t3(b.x, b.y, b.z), t4(a.x, b.y, b.z);

            engine.draw_quad(b1, b2, b3, b4); engine.draw_quad(t4, t3, t2, t1);
            engine.draw_quad(b2, b1, t1, t2); engine.draw_quad(b3, b2, t2, t3);
            engine.draw_quad(b4, b3, t3, t4); engine.draw_quad(b1, b4, t4, t1);
            } break;

        case SPHERE: {
            engine.set_material(command.op.sphere.constants);

            Vec3d pos(command.op.sphere.d);
            Float64 radius(command.op.sphere.r);

            const Float64 dp = Math::PI / kSegments;
            const Float64 dt = Math::PI / kSegments;
            for(Float64 phi = 0.0; phi < Math::PI; phi += dp)
            {
                for(Float64 theta = 0.0; theta <= Math::TAU; theta += dt)
                {
                    Vec3d da = pos + radius * Vec3d(
                        std::cos(phi),
                        std::sin(phi) * std::cos(theta),
                        std::sin(phi) * std::sin(theta)
                    );

                    Vec3d db = pos + radius * Vec3d(
                        std::cos(phi + dp),
                        std::sin(phi + dp) * std::cos(theta),
                        std::sin(phi + dp) * std::sin(theta)
                    );

                    Vec3d dc = pos + radius * Vec3d(
                        std::cos(phi + dp),
                        std::sin(phi + dp) * std::cos(theta + dt),
                        std::sin(phi + dp) * std::sin(theta + dt)
                    );

                    Vec3d dd = pos + radius * Vec3d(
                        std::cos(phi),
                        std::sin(phi) * std::cos(theta + dt),
                        std::sin(phi) * std::sin(theta + dt)
                    );

                    engine.draw_quad(da, db, dc, dd);
                }
            }
            } break;

        case TORUS: {
            engine.set_material(command.op.torus.constants);

            Vec3d pos(command.op.torus.d);
            Float64 radius1(command.op.torus.r0);
            Float64 radius2(command.op.torus.r1);

            const Float64 dp = Math::TAU / kSegments;
            const Float64 dt = Math::TAU / kSegments;
            for(Float64 phi = 0.0; phi <= Math::TAU; phi += dp)
            {
                for(Float64 theta = 0.0; theta <= Math::TAU; theta += dt)
                {
                    Vec3d da = pos + Vec3d(
                        radius2 * std::cos(phi) + radius1 * std::cos(phi) * std::cos(theta + dt),
                        radius1 * std::sin(theta + dt),
                        radius2 * std::sin(phi) + radius1 * std::sin(phi) * std::cos(theta + dt)
                    );

                    Vec3d db = pos + Vec3d(
                        radius2 * std::cos(phi + dp) + radius1 * std::cos(phi + dp) * std::cos(theta + dt),
                        radius1 * std::sin(theta + dt),
                        radius2 * std::sin(phi + dp) + radius1 * std::sin(phi + dp) * std::cos(theta + dt)
                    );

                    Vec3d dc = pos + Vec3d(
                        radius2 * std::cos(phi + dp) + radius1 * std::cos(phi + dp) * std::cos(theta),
                        radius1 * std::sin(theta),
                        radius2 * std::sin(phi + dp) + radius1 * std::sin(phi + dp) * std::cos(theta)
                    );

                    Vec3d dd = pos + Vec3d(
                        radius2 * std::cos(phi) + radius1 * std::cos(phi) * std::cos(theta),
                        radius1 * std::sin(theta),
                        radius2 * std::sin(phi) + radius1 * std::sin(phi) * std::cos(theta)
                    );

                    engine.draw_quad(da, db, dc, dd);
                }
            }
            } break;
        
        case SCALE: {
            Vec3d scale(command.op.scale.d);
            engine.modify_transform(Mat4d::Scale(scale));
            } break;

        case MOVE: {
            Vec3d translate(command.op.move.d);
            engine.modify_transform(Mat4d::Translation(translate));
            } break;

        case ROTATE: {
            int axis = std::lround(command.op.rotate.axis);
            Float64 theta(command.op.rotate.degrees);
            theta = Math::PI * theta / 180.0;

            /**/ if(axis == 0) engine.modify_transform(Mat4d::RotX(theta));
            else if(axis == 1) engine.modify_transform(Mat4d::RotY(theta));
            else if(axis == 2) engine.modify_transform(Mat4d::RotZ(theta));
            else std::cout << "Unknown Axis \"" << axis << "\". Ignoring...\n";

            } break;

        case DISPLAY: {
            std::ofstream file;
            std::string temp_file_name = ".display_tmp_" + std::to_string(i) + ".ppm";
            file.open(temp_file_name, std::ios::binary);

            Image img = FXAA::apply(engine.image());
            file << img;
            file.close();

            std::system(("open " + temp_file_name).c_str());
            } break;

        case SAVE: {
            std::string file_name(command.op.save.p->name);
            std::string temp_file_name = file_name + ".ppm";

            std::ofstream file;
            file.open(temp_file_name.c_str(), std::ios::binary | std::ios::trunc);
            file << FXAA::apply(engine.image());
            file.close();

            std::system(("convert " + temp_file_name + " " + file_name + " && rm -f " + temp_file_name).c_str());
            } break;

        default: {
            std::cout << "Unknown OP Code: " << command.opcode;
            } break;
        }

        std::cout << "\n";
    }
}

/**
 * 
//   int i;
//   struct matrix *tmp;
//   struct stack *systems;
//   screen t;
//   zbuffer zb;
//   color g;
//   double step_3d = 20;
//   double theta;

//   //Lighting values here for easy access
//   color ambient;
//   ambient.red = 50;
//   ambient.green = 50;
//   ambient.blue = 50;

//   double light[2][3];
//   light[LOCATION][0] = 0.5;
//   light[LOCATION][1] = 0.75;
//   light[LOCATION][2] = 1;

//   light[COLOR][RED] = 0;
//   light[COLOR][GREEN] = 255;
//   light[COLOR][BLUE] = 255;

//   double view[3];
//   view[0] = 0;
//   view[1] = 0;
//   view[2] = 1;

//   //default reflective constants if none are set in script file
//   struct constants white;
//   white.r[AMBIENT_R] = 0.1;
//   white.g[AMBIENT_R] = 0.1;
//   white.b[AMBIENT_R] = 0.1;

//   white.r[DIFFUSE_R] = 0.5;
//   white.g[DIFFUSE_R] = 0.5;
//   white.b[DIFFUSE_R] = 0.5;

//   white.r[SPECULAR_R] = 0.5;
//   white.g[SPECULAR_R] = 0.5;
//   white.b[SPECULAR_R] = 0.5;

//   //constants are a pointer in symtab, using one here for consistency
//   struct constants *reflect;
//   reflect = &white;

//   systems = new_stack();
//   tmp = new_matrix(4, 1000);
//   clear_screen( t );
//   clear_zbuffer(zb);
//   g.red = 0;
//   g.green = 0;
//   g.blue = 0;
*/
