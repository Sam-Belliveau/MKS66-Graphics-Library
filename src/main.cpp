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

    print_symtab();

    std::string basename = "default";
    int frames = 1;

    std::unordered_map<std::string, std::vector<double>> table;

    for (int i = 0; i < lastop; i++)
    {
        struct command& command = op[i];

        switch(command.opcode)
        {
        case BASENAME: {
            basename = command.op.basename.p->name;
            std::cout << i << ": Basename = " << basename << std::endl;
        }; break;

        case FRAMES: {
            frames = command.op.frames.num_frames;
            std::cout << i << ": Number of Frames = " << frames << std::endl;
        }; break;

        default: {} break;
        }
    }

    for (int i = 0; i < lastop; i++)
    {
        struct command& command = op[i];

        if(command.opcode == VARY)
        {
            std::vector<double> values;
            for (int f = 0; f < frames; ++f) {
                if (f <= command.op.vary.start_frame) 
                { values.push_back(command.op.vary.start_val); }
                else if (f >= command.op.vary.end_frame) 
                { values.push_back(command.op.vary.end_val); }

                else 
                {
                    double t = (f - command.op.vary.start_frame) / double(command.op.vary.end_frame - command.op.vary.start_frame);
                    values.push_back(
                        command.op.vary.start_val * (1.0 - t) + 
                        command.op.vary.end_val * t
                    );
                }
            }
            table[command.op.vary.p->name] = values;
        }
    }
    
    Engine engine(500, 500);

    for (int f = 0; f < frames; ++f)
    {
        std::cerr << "Frame " << f << " / " << frames << "...";  
        engine.reset();
        for (int i = 0; i < lastop; i++) 
        {
            struct command& command = op[i];

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

                if (command.op.scale.p != NULL
                && table.contains(command.op.scale.p->name))
                { scale *= table[command.op.scale.p->name][f]; }

                engine.modify_transform(Mat4d::Scale(scale));
                } break;

            case MOVE: {
                Vec3d translate(command.op.move.d);

                if (command.op.move.p != NULL
                && table.contains(command.op.move.p->name))
                { translate *= table[command.op.move.p->name][f]; }

                engine.modify_transform(Mat4d::Translation(translate));
                } break;

            case ROTATE: {
                int axis = std::lround(command.op.rotate.axis);
                Float64 theta(command.op.rotate.degrees);
                theta = Math::PI * theta / 180.0;

                if (command.op.rotate.p != NULL
                && table.contains(command.op.rotate.p->name))
                { theta *= table[command.op.rotate.p->name][f]; }

                /**/ if(axis == 0) engine.modify_transform(Mat4d::RotX(theta));
                else if(axis == 1) engine.modify_transform(Mat4d::RotY(theta));
                else if(axis == 2) engine.modify_transform(Mat4d::RotZ(theta));
                else std::cout << "Unknown Axis \"" << axis << "\". Ignoring...\n";

                } break;

            case DISPLAY: {
                std::string temp_file_name = ".display_tmp_" + std::to_string(i) + ".ppm";
                engine.save(temp_file_name);
                std::system(("open " + temp_file_name).c_str());
                } break;

            case SAVE: {
                engine.save(command.op.save.p->name);
                } break;

            default: {
                std::cout << "Unknown OP Code: " << command.opcode;
                } break;
            }
        }

        if (frames != 1)
        {
            char fname[100] = {};
            sprintf(fname, "%s-%04d.png", basename.c_str(), f);
            engine.save(std::string(fname));
        }

        std::cerr << "Done!\n";
    }
    
    if (frames != 1)
    {
        std::string cmd = "convert -delay 10 ./";
        cmd += basename;
        cmd += "-*";
        cmd += " ";
        cmd += basename;
        cmd += ".gif && rm ./";
        cmd += basename;
        cmd += "-*";
        std::system(cmd.c_str());
    }
}