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
	glPopMatrix();
}