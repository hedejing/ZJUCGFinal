/*  测试rotateMat函数是否正常工作  */
void test1() {
	glPushMatrix();
		glLoadIdentity();
		glRotated(45, -5, 10, 0);
		//glRotated(45, 1, 0, 0);
		double mat[16];
		glGetDoublev(GL_MODELVIEW_MATRIX, mat);
		for (int i=0; i<16; i++) cout<<mat[i]<<char(i%4==3?10:32);

		cout<<endl;

		glLoadIdentity();
		glMultMatrixd(rotateMat(45, -5, 10, 0));
		glGetDoublev(GL_MODELVIEW_MATRIX, mat);
		for (int i=0; i<16; i++) cout<<mat[i]<<char(i%4==3?10:32);


		cout<<endl;

		glLoadIdentity();
		GLMat matt = rotateQuat(45, -5, 10, 0);
		glMultMatrixd(matt);
		glGetDoublev(GL_MODELVIEW_MATRIX, mat);
		for (int i=0; i<16; i++) cout<<mat[i]<<char(i%4==3?10:32);
	glPopMatrix();
}
void test2() {
	double mat[16];
	glPushMatrix();
		glLoadIdentity();
		gluLookAt(1, 1, 1, 2, 3, 3, 0, 1, 0);
		glGetDoublev(GL_MODELVIEW_MATRIX, mat);
		for (int i=0; i<16; i++) cout<<mat[i]<<char(i%4==3?10:32);

		cout<<endl;

		glLoadIdentity();
		glMultMatrixd(lookAtMat(1, 1, 1, 2, 3, 3, 0, 1, 0));
		glGetDoublev(GL_MODELVIEW_MATRIX, mat);
		for (int i=0; i<16; i++) cout<<mat[i]<<char(i%4==3?10:32);
	glPopMatrix();
	glPushMatrix();
		glLoadIdentity();
		gluPerspective(45, 1.5, 0.1, 100);
		glGetDoublev(GL_MODELVIEW_MATRIX, mat);
		for (int i=0; i<16; i++) cout<<mat[i]<<char(i%4==3?10:32);

		cout<<endl;

		glLoadIdentity();
		glMultMatrixd(perspectiveMat(45, 1.5, 0.1, 100));
		glGetDoublev(GL_MODELVIEW_MATRIX, mat);
		for (int i=0; i<16; i++) cout<<mat[i]<<char(i%4==3?10:32);
	glPopMatrix();
}

void test3() {
	GLMat mat = lookAtMat(1,100,1, 2,2,3, 20,1,0);
	cout<<mat<<endl;
	cout<<mat.affineInverse()<<endl;
	cout<<mat.affineInverse()*mat<<endl;
	cout<<mat*mat.affineInverse()<<endl;
}