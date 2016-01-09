#include"Model.h"
#include<fstream>
using namespace std;
void Mtl_info::change_MTL(string mtl_name)
{
	A_mtl ma = mtls[mtl_name];
	if (ma.content&IF_KA)
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ma.ka);
	}
	if (ma.content&IF_KD)
	{
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE, ma.kd);
	}
	if (ma.content&IF_KS)
	{
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR, ma.ks);
	}
	if (ma.content&IF_KE)
	{
		//glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, ma.ke);
	}
	if (ma.content&IF_IL)
	{
		
	}
	if (ma.content&IF_KD_TE)
	{
		glDisable(GL_TEXTURE_2D);
		//cout << mtl_name << endl;;
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, ma.Kd_Texture_ID);
	}
}
Mtl_info::Mtl_info(const char *filename,string path)//读取mtl文件信息
{
	fstream fin;
	fin.open(filename, ios::in);
	string s;
	A_mtl *tmp=NULL;
	string mtl_name;
	while (fin >> s)
	{
		//cout << s << endl;;
		if (s == "newmtl")
		{
			if (tmp)
				mtls[mtl_name] = *tmp;
			tmp = new A_mtl;
			tmp->content = 0;
			fin >> mtl_name;
			continue;
		}else 
		if (s == "illum")
		{
			tmp->content += IF_IL;
			fin >> tmp->illum_type;
			continue;
		}else
		if (s == "Kd")
		{
			tmp->content += IF_KD;
			fin >> tmp->kd[0] >> tmp->kd[1] >> tmp->kd[2];
		}else
		if (s == "Ka") 
		{
			tmp->content += IF_KA;
			fin >> tmp->ka[0] >> tmp->ka[1] >> tmp->ka[2]; 
		}else
		if (s == "Ks")
		{
			tmp->content += IF_KS;
			fin >> tmp->ks[0] >> tmp->ks[1] >> tmp->ks[2];
		}else
		if (s == "Ke")
		{
			tmp->content += IF_KE;
			fin >> tmp->ke[0] >> tmp->ke[1] >> tmp->ke[2];
		}else
		if (s == "Ns")
		{
			tmp->content += IF_NS;
			fin >> tmp->ns;
		}else
		if (s == "map_Kd")
		{
			tmp->content += IF_KD_TE;
			string t_file;
			fin >> t_file;
			t_file = path + t_file;
			if (t_file.substr(t_file.length() - 3) == "tga")
			{
				tmp->Kd_Texture_ID = Model::LoadTGA(t_file.c_str());
			}
			if (t_file.substr(t_file.length() - 3) == "bmp")
			{
				tmp->Kd_Texture_ID = loadTexture(t_file.c_str());
			}
		}
	else
		getline(fin, s);//过滤掉多余信息
	}
	if (tmp)
		mtls[mtl_name] = *tmp;
	fin.close();
}
Mtl_info::~Mtl_info()
{

}
void Model::draw()
{
	int i = 0;
	//glEnable(GL_COLOR_MATERIAL);
	for (auto ID : DrawListID)
	{
		obj_mtl->change_MTL(ID_NAMES[i++]);
		glCallList(ID);
	}
	//glDisable(GL_COLOR_MATERIAL);
	float am[4] = { 0.3, 0.3, 0.3, 1 };
	float nor[4] = { 0.5, 0.5, 0.5, 1 };
	float zero[4] = { 0.3, 0.3, 0.3, 1 };

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, am);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, zero);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, nor);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 70.0);
}
Model::Model()
{
	
}
GLuint Model::LoadTGA(const char *filename)
{
	GLubyte     TGAheader[12] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };    // Uncompressed TGA Header  
	GLubyte     TGAcompare[12];                             // Used To Compare TGA Header  
	GLubyte     header[6];                                  // First 6 Useful Bytes From The Header  
	GLuint      bytesPerPixel;                              // Holds Number Of Bytes Per Pixel Used In The TGA File  
	GLuint      imageSize;                                  // Used To Store The Image Size When Setting Aside Ram  
	GLuint      temp;                                       // Temporary Variable  
	GLuint      type = GL_RGBA;                               // Set The Default GL Mode To RBGA (32 BPP)  
	GLuint		texture_ID;
	glGenTextures(1, &texture_ID);
	FILE *file = fopen(filename, "rb");                     // Open The TGA File  

	if (file == NULL ||                                       // Does File Even Exist?  
		fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare) ||  // Are There 12 Bytes To Read?  
		memcmp(TGAheader, TGAcompare, sizeof(TGAheader)) != 0 ||  // Does The Header Match What We Want?  
		fread(header, 1, sizeof(header), file) != sizeof(header))                // If So Read Next 6 Header Bytes  
	{
		if (file == NULL)                                   // Did The File Even Exist? *Added Jim Strong*  
			return false;                                   // Return False  
		else
		{
			fclose(file);                                   // If Anything Failed, Close The File  
			return false;                                   // Return False  
		}
	}
	int width, height;
	width = header[1] * 256 + header[0];           // Determine The TGA Width  (highbyte*256+lowbyte)  
	height = header[3] * 256 + header[2];           // Determine The TGA Height (highbyte*256+lowbyte)  

	//OpenGL中纹理只能使用24位或者32位的TGA图像  
	if (width <= 0 ||                              // Is The Width Less Than Or Equal To Zero  
		height <= 0 ||                              // Is The Height Less Than Or Equal To Zero  
		(header[4] != 24 && header[4] != 32))                   // Is The TGA 24 or 32 Bit?  
	{
		fclose(file);                                       // If Anything Failed, Close The File  
		return false;                                       // Return False  
	}

	int bpp = header[4];                            // Grab The TGA's Bits Per Pixel (24 or 32)  
	bytesPerPixel = bpp / 8;                        // Divide By 8 To Get The Bytes Per Pixel  
	imageSize = width*height*bytesPerPixel;   // Calculate The Memory Required For The TGA Data  

	GLubyte *imageData = (GLubyte *)malloc(imageSize);     // Reserve Memory To Hold The TGA Data  

	if (imageData == NULL ||                          // Does The Storage Memory Exist?  
		fread(imageData, 1, imageSize, file) != imageSize)    // Does The Image Size Match The Memory Reserved?  
	{
		if (imageData != NULL)                     // Was Image Data Loaded  
			free(imageData);                        // If So, Release The Image Data  

		fclose(file);                                       // Close The File  
		return false;                                       // Return False  
	}

	//RGB数据格式转换，便于在OpenGL中使用  
	for (GLuint i = 0; i<int(imageSize); i += bytesPerPixel)      // Loop Through The Image Data  
	{                                                       // Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)  
		temp = imageData[i];                          // Temporarily Store The Value At Image Data 'i'  
		imageData[i] = imageData[i + 2];    // Set The 1st Byte To The Value Of The 3rd Byte  
		imageData[i + 2] = temp;                    // Set The 3rd Byte To The Value In 'temp' (1st Byte Value)  
	}

	fclose(file);                                          // Close The File  

	// Build A Texture From The Data               
	glBindTexture(GL_TEXTURE_2D, texture_ID);         // Bind Our Texture  
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   // Linear Filtered  
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // Linear Filtered  

	if (bpp == 24)                                 // Was The TGA 24 Bits  
	{
		type = GL_RGB;                                        // If So Set The 'type' To GL_RGB  
	}

	glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, imageData);

	return texture_ID;
}
Model::Model(const Model &another)
{
	obj_mtl = another.obj_mtl;
	DrawListID=another.DrawListID;
}
void Model::Readobj(const char * filename)
{
ifstream fin;
	fin.open(filename,ifstream::in);
	if (!fin) {
	//	throw Model_ERROR2;
		return;}
	string path = filename;
	int tmp = path.length()-1;
	while (tmp >= 0 && path[tmp] != '/' && path[tmp] != '\\')
	{
		tmp--;
	}
	path.erase(tmp+1);
	string s;
	group *mt_g = NULL,*tmp_g=NULL;
	while (fin>>s)
	{
		if (s == "mtllib")
		{
			string filename;
			fin >> filename;
			obj_mtl = new Mtl_info((path+filename).c_str(),path);
			continue;
		}
		if (s == "usemtl")
		{
			if (mt_g)
			{
				mt_g->end_index = faces.size() - 1;
				mtl_group.push_back(*mt_g);
			}
			mt_g = new group;
			mt_g->begin_index = faces.size();
			fin >> mt_g->name;
			continue;
		}
		switch(*s.c_str())
		{
		case 'g':
			{
					if (tmp_g)
					{
						tmp_g->end_index = faces.size() - 1;
						g.push_back(*tmp_g);
					}
					tmp_g = new group;
					tmp_g->begin_index = faces.size();
					fin >> tmp_g->name;
			}
			break;
		case '#':
			 getline(fin,s);
			break;
		case 'v':
			 if (*(s.c_str()+1)=='n')//法线向量
			 {
				v vertex;
				fin>>vertex.x>>vertex.y>>vertex.z;
				vert_nor.push_back(vertex);
			 }
			 else if (*(s.c_str()+1)=='t')//纹理坐标
			 {
				vt vertex;
				fin>>vertex.x>>vertex.y;
				vertex.y -= 1;
				texture_ord.push_back(vertex);
			 }
			 else//顶点
			 {
				v vertex;
				fin>>vertex.x>>vertex.y>>vertex.z;
				if (!getC)
				{
					getC = 1;
					minx = maxx = vertex.x;
					miny = maxy = vertex.y;
					minz = maxz = vertex.z;
				}
				else
				{
					if (minx > vertex.x) minx = vertex.x;
					if (maxx < vertex.x)maxx = vertex.x;
					if (miny>vertex.y)miny = vertex.y;
					if (maxy < vertex.y)maxy = vertex.y;
					if (minz >vertex.z) minz = vertex.z;
					if (maxz < vertex.z) maxz = vertex.z;
				}
				vertices.push_back(vertex);
			 }
			break;
		case 'f'://面信息
			{
				f face,tex,normal;

				char useless;
				fin>>face.v1>>useless>>tex.v1>>useless>>normal.v1//过滤掉斜杠
					>>face.v2>>useless>>tex.v2>>useless>>normal.v2
					>>face.v3>>useless>>tex.v3>>useless>>normal.v3;
				faces.push_back(face);
				face_tex.push_back(tex);
				normals.push_back(normal);
					break;
			}
		}
	}
	fin.close();
	if (mt_g)
	{
		mt_g->end_index = faces.size() - 1;
		mtl_group.push_back(*mt_g);
	}
	if (tmp_g)
	{
		tmp_g->end_index = faces.size() - 1;
		g.push_back(*tmp_g);
	}
	int now_mtl = -1;
	if (mtl_group.size() > 0)
	{	
		now_mtl = 0;
		obj_mtl->change_MTL(mtl_group[now_mtl].name);
		ID_NAMES.push_back(mtl_group[now_mtl].name);
	}
	GLuint tmpID = glGenLists(1);
	glDisable(GL_TEXTURE_2D);
	glNewList(tmpID,GL_COMPILE);
	glBegin(GL_TRIANGLES);
	for (int i=0;i<faces.size();i++)
	{
		v v1=vertices[faces[i].v1-1];
		v v2=vertices[faces[i].v2-1];
		v v3=vertices[faces[i].v3-1];
		vt tex1=texture_ord[face_tex[i].v1-1];
		vt tex2=texture_ord[face_tex[i].v2-1];
		vt tex3=texture_ord[face_tex[i].v3-1];
		v nor1=vert_nor[normals[i].v1-1];
		v nor2=vert_nor[normals[i].v2-1];
		v nor3=vert_nor[normals[i].v3-1];
		
		if (now_mtl > -1 && i > mtl_group[now_mtl].end_index)
		{
			glEnd();
			DrawListID.push_back(tmpID);
			glEndList();
			tmpID = glGenLists(1);
			now_mtl++;
			obj_mtl->change_MTL(mtl_group[now_mtl].name);
			ID_NAMES.push_back(mtl_group[now_mtl].name);
			glNewList(tmpID, GL_COMPILE);
			glBegin(GL_TRIANGLES);
		}
		
		glNormal3f(nor1.x,nor1.y,nor1.z);//第一个顶点
		glTexCoord2f(tex1.x,tex1.y);
		glVertex3f(v1.x,v1.y,v1.z);
		glNormal3f(nor2.x,nor2.y,nor2.z);//第二个顶点
		glTexCoord2f(tex2.x,tex2.y);
		glVertex3f(v2.x,v2.y,v2.z);
		glNormal3f(nor3.x,nor3.y,nor3.z);//第三个顶点
		glTexCoord2f(tex3.x,tex3.y);
		glVertex3f(v3.x,v3.y,v3.z);
		
	}
	
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glEndList();
	DrawListID.push_back(tmpID);
	
	vertices.clear();
	faces.clear();
	face_tex.clear();
	texture_ord.clear();
	normals.clear();
}
Model::~Model()
{
	//glDeleteLists(DrawListID,1);
	//if (obj_mtl)delete obj_mtl;
}