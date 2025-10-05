#include <GL/freeglut.h>
#include <iostream>

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
        glVertex2f(-0.5, -0.5);
        glVertex2f(0.5, -0.5);
        glVertex2f(0.5, 0.5);
        glVertex2f(-0.5, 0.5);
    glEnd();
    glutSwapBuffers();
}

int main(int argc, char** argv) {
    std::cout << "Starting GLUT test program..." << std::endl;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("GLUT Test");
    glutDisplayFunc(display);
    std::cout << "GLUT initialized successfully!" << std::endl;
    glutMainLoop();
    return 0;
}