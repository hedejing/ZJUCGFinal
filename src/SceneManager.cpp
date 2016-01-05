#include "SceneManager.h"

using namespace std;


SceneManager::SceneManager(std::string filename)
{
	ifstream in(filename);
	data << in;
	in.close();
}

void SceneManager::LoadScenes(int SceneId)
{
	try {
		int SceneNum = data["SceneNum"];
		if (SceneId >= SceneNum)
		{
			puts("Given scene id exceeds number of scene!");
			throw invalid_argument("Given scene id exceeds number of scene!");
		}
		json scene = data["Scenes"][SceneId];
		json camera, eye, center;
		// set up camera
		if(scene.find("Camera") == scene.end())
			throw invalid_argument("can't find 'Camera'!!!");
		camera = scene["Camera"];
		if(camera.find("eye") == camera.end())
			throw invalid_argument("can't find 'eye of Camera'!!!");
		if(camera.find("center") == camera.end())
			throw invalid_argument("can't find 'center of Camera!!!");
		eye = camera["eye"];
		center = camera["center"];
		World::setCamera(Point(eye[0], eye[1], eye[2]), Point(center[0], center[1], center[2]));

		// set up lights
		int LightNum = scene["LightNum"];
		json lights = scene["Lights"];
		for (int i = 0; i < LightNum; i++)
		{
			if (lights[i].find("position")==lights[i].end())
				throw invalid_argument("can't find position");
			json position = lights[i]["position"];
			LightSource ls(Point(position[0], position[1], position[2], position[3]));
			LightManager::addLight(ls);
			if(lights[i].find("ambient")!=lights[i].end())
			{
				json ambient = lights[i]["ambient"];
				glLightfv(GL_LIGHT0 + i, GL_AMBIENT, Color(ambient[0], ambient[1], ambient[2], ambient[3]));
			}
			if (lights[i].find("diffuse") != lights[i].end())
			{
				json diffuse = lights[i]["diffuse"];
				glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, Color(diffuse[0], diffuse[1], diffuse[2], diffuse[3]));
			}
			if (lights[i].find("specular") != lights[i].end())
			{
				json specular = lights[i]["specular"];
				glLightfv(GL_LIGHT0 + i, GL_SPECULAR, Color(specular[0], specular[1], specular[2], specular[3]));
			}
		}

		// set up objects
		int ObjectNum = scene["ObjectNum"];
		json objects = scene["Objects"];
		SetupObjects(objects, ObjectNum);
	}
	catch (const exception &e)
	{
		std::cout << e.what() << std::endl;
	}
}

void SceneManager::SetupObjects(const json & objects, int ObjectNum)
{

	for (int i = 0; i < ObjectNum; i++)
	{
		string type = objects[i]["type"];
		json centroidJson = objects[i]["centroid"];
		Point centroid;
		if (!centroidJson.is_null())
			centroid = Point(centroidJson[0], centroidJson[1], centroidJson[2]);
		else
			throw invalid_argument("can't find 'centroid' of object");

		auto rotateQuatJson = objects[i].find("rotateQuat");
		auto scaleValueJson = objects[i].find("scaleValue");

		Quat rotateQuat;
		if (rotateQuatJson!=objects[i].end())
			rotateQuat = Quat((*rotateQuatJson)[0], (*rotateQuatJson)[1], (*rotateQuatJson)[2], (*rotateQuatJson)[3]);
		Vec scaleValue;
		if (scaleValueJson!=objects[i].end())
			scaleValue = Vec((*scaleValueJson)[0], (*scaleValueJson)[1], (*scaleValueJson)[2]);

		
		if (type == "Cube") {
			Cube *cube = new Cube(centroid);
			cube->centroid = centroid;
			if (rotateQuatJson!=objects[i].end())
				cube->rotateQuat = rotateQuat;
			if (scaleValueJson!=objects[i].end())
				cube->scaleValue = scaleValue;
		}
		else if (type == "Cone") {
			Cone *cone = new Cone(centroid);
			cone->centroid = centroid;
			if (rotateQuatJson!=objects[i].end())
				cone->rotateQuat = rotateQuat;
			if (scaleValueJson!=objects[i].end())
				cone->scaleValue = scaleValue;
			if(objects[i].find("radius")!=objects[i].end())
				cone->radius = objects[i]["radius"];
			if(objects[i].find("height")!=objects[i].end())
				cone->height = objects[i]["height"];
			if(objects[i].find("slices")!=objects[i].end())
				cone->slices = objects[i]["slices"];
			if(objects[i].find("stacks")!=objects[i].end())
				cone->stacks = objects[i]["stacks"];
		}
		else if (type == "Sphere") {
			Sphere *sphere = new Sphere(centroid);
			if (rotateQuatJson!=objects[i].end())
				sphere->rotateQuat = rotateQuat;
			if (scaleValueJson!=objects[i].end())
				sphere->scaleValue = scaleValue;
			if(objects[i].find("radius")!=objects[i].end())
				sphere->radius = objects[i]["radius"];
			if(objects[i].find("slices")!=objects[i].end())
				sphere->slices = objects[i]["slices"];
			if(objects[i].find("stacks")!=objects[i].end())
				sphere->stacks = objects[i]["stacks"];
		}
		else if(type == "Circle"){
			Circle *circle = new Circle(centroid);
			if (rotateQuatJson!=objects[i].end())
				circle->rotateQuat = rotateQuat;
			if (scaleValueJson!=objects[i].end())
				circle->scaleValue = scaleValue;
			if(objects[i].find("radius")!=objects[i].end())
				circle->radius = objects[i]["radius"];
			if(objects[i].find("slices")!=objects[i].end())
				circle->slices = objects[i]["slices"];
		}
		else if (type == "Cylinder") {
			Cylinder *cylinder = new Cylinder(centroid);
			if (rotateQuatJson!=objects[i].end())
				cylinder->rotateQuat = rotateQuat;
			if (scaleValueJson!=objects[i].end())
				cylinder->scaleValue = scaleValue;
			if(objects[i].find("radius")!=objects[i].end())
				cylinder->radius = objects[i]["radius"];
			if(objects[i].find("height")!=objects[i].end())
				cylinder->height = objects[i]["height"];
			if(objects[i].find("slices")!=objects[i].end())
				cylinder->slices = objects[i]["slices"];
			if(objects[i].find("stacks")!=objects[i].end())
				cylinder->stacks = objects[i]["stacks"];
		}
	}

}

void SceneManager::DumpScenes(string filename)
{
	ofstream out(filename);
	out << setw(4) << data;
	out.close();
}


SceneManager::~SceneManager()
{
}
