#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML\Graphics.hpp>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <cmath>


#define WAY_TO_IMAGE "D:/TestSFML/src/test.png"
#define WAY_TO_TXT "D:/TestSFML/src/input.txt"

const float pi = 3.1415926;
const int count_of_points = 360;
const int R = 150;

sf::RenderWindow screen(sf::VideoMode(1200, 800), "window");
sf::VertexArray lines(sf::Lines, count_of_points /*количество точек*/);

class NotColor {
public:
	NotColor(int r = 0, int g = 0, int b = 0, int a = 255) {

	}
	int r = 0, g = 0, b = 0, a = 255;
	bool NotColor::operator==(const NotColor& other) {
		if (r == other.r & g == other.g & b == other.b & a == other.a) return true;
		else return false;
	}

};

class Pixel {
public:
	Pixel::Pixel(int x = 0, int y = 0) : x(x), y(y)
	{}
	int x;
	int y;
	sf::Color color;
};

class Point {
	
public:
	double x, y;
	Point(double _x = 0, double _y = 0) : x(_x), y(_y) {}

	double Point::GetX() {
		return x;
	}
	void Point::SetX(double valueX) {
		x = valueX;
	}
	double Point::GetY() {
		return y;
	}
	void Point::SetY(double valueY) {
		y = valueY;
	}
};

class Lens final {
	double focus;
	double R1, R2;
	double n_lens, n_environment, object_distance;
public:
	Lens(double _focus = 0, double _R1 = 0, double _R2 = 0, double _n_lens = 0, double _n_environment = 0, double _object_distance = 0) :
		focus(_focus), R1(_R1), R2(_R2), n_lens(_n_lens), n_environment(_n_environment), object_distance(_object_distance) {

	}

	double Lens::GetFocus() {
		return focus;
	}
	void Lens::SetFocus(double valueFocus) {
		focus = valueFocus;
	}
	double Lens::GetR1() {
		return R1;
	}
	void Lens::SetR1(double valueR1) {
		R1 = valueR1;
	}
	double Lens::GetR2() {
		return R1;
	}
	void Lens::SetR2(double valueR2) {
		R2 = valueR2;
	}
	double Lens::GetNLens() {
		return n_lens;
	}
	void Lens::SetNLens(double valueNLens) {
		n_lens = valueNLens;
	}
	double Lens::GetNEnvironment() {
		return n_environment;
	}
	void Lens::SetNEnvironment(double valueNEnvironment) {
		n_environment = valueNEnvironment;
	}

	void Lens::SetHandleFocus() {
		focus = 1 / ((n_lens - 1) * (1 / R1 + 1 / R2));
	}

	double Lens::GetObjectDistance() {
		return object_distance;
	}
	void Lens::SetObjectDistance(double value_object_distance) {
		object_distance = value_object_distance;
	}

};

double ParaxConvert(Lens& lens, Point& point) {
	Point image(0, 0);
	double gamma = 0;
	image.SetX((lens.GetFocus() * abs(point.GetX())) / (abs(point.GetX() - lens.GetFocus())));
	image.SetY(image.GetX() * abs(point.GetY()) / point.GetX());
	//x = image.GetX();
	//y = image.GetY();
	gamma = image.GetY() / point.GetY();
	return gamma;
}

double NonParaxConvert(Lens& lens, Point& point) {
	Point image(0, 0);
	double gamma = 0;
	double slope_factor = point.GetY() / point.GetX();
	double koef1 = (1 + pow(slope_factor, 2)) / (lens.GetFocus() * (lens.GetNLens() - 1));
	double koef2 = sqrt(pow(lens.GetNLens(), 2) + (pow(lens.GetNLens(), 2) - 1) * pow(slope_factor, 2)) - 1;
	double denominator = 1 / point.GetX() + koef1 * koef2;
	image.SetX(1 / denominator);
	image.SetY(slope_factor * image.GetX());
	//x = image.GetX();
	//y = image.GetY();
	gamma = image.GetY() / point.GetY();
	return gamma;
}

void pixelConvert(int RAD, sf::Vector2i mouse_position,
	std::vector <Pixel>& objects,
	std::vector < std::vector < NotColor> >& matrix, sf::Vector2i limits, Lens lens, Point point) {
	double gamma = 0;
	NonParaxConvert(lens, point);
	RAD = RAD / gamma;
	Pixel pixel;
	sf::Color color;

	for (int i = -RAD; i < RAD; i++) {
		for (int j = -RAD; j < RAD; j++) {
			if (i * i + j * j < RAD * RAD & mouse_position.x + i + RAD < limits.x & mouse_position.x + i + RAD > 0 & j + RAD + mouse_position.y < limits.y & j + RAD + mouse_position.y > RAD) {
				point.x = mouse_position.x + i;
				point.y = mouse_position.y + j;

				gamma = NonParaxConvert(lens, point);
				std::cout << gamma;
				pixel = { (int)(mouse_position.x + gamma * i), (int)(mouse_position.y + gamma * j) };
				pixel.color.r = (sf::Uint8)matrix[mouse_position.x + i][mouse_position.y + j].r;
				pixel.color.g = (sf::Uint8)matrix[mouse_position.x + i][mouse_position.y + j].g;
				pixel.color.b = (sf::Uint8)matrix[mouse_position.x + i][mouse_position.y + j].b;
				pixel.color.a = (sf::Uint8)matrix[mouse_position.x + i][mouse_position.y + j].a;
				objects.push_back(pixel);
			}
		}
	}
}


class Background {


	sf::Vector2u size_of_pic;
public:
	Background(std::string name_of_pic) {
		sf::Texture texture;
		if (!texture.loadFromFile(name_of_pic)) {
			std::cout << "Error loading texture" << std::endl;
		}

		size_of_pic = texture.getSize();
	}


	void loadTexture(std::string name_of_pic) {
		sf::Sprite background;
		sf::Texture texture;

		if (!texture.loadFromFile(name_of_pic)) {
			std::cout << "Error loading texture" << std::endl;
		}
		background.setTexture(texture);
		screen.draw(background);
		//size_of_pic = texture.getSize();

	}

	sf::Vector2u getPicSize() {
		sf::Vector2u size;
		size = size_of_pic;
		return size;
	}

	void prepare(std::string name_of_pic, std::string name_of_file) {					//Takes the names of the image file and the file where to write and 
																						//writes the coordinates of each pixel and its color to the file
		sf::Image texture;
		if (!texture.loadFromFile(name_of_pic)) {
			std::cout << "Can't open image";
			return;
		}
		sf::Color color;
		std::ofstream pixels;
		pixels.open(name_of_file);

		if (!pixels.is_open()) {
			std::cout << "Can't open file";
			return;
		}

		for (int i = 0; i < texture.getSize().x; i++) {
			for (int j = 0; j < texture.getSize().y; j++) {
				color = texture.getPixel(i, j);
				pixels << (int)color.r << " " << (int)color.g << " " << (int)color.b << " " << (int)color.a << std::endl;
			}
		}
		pixels.close();
		//size_of_pic = texture.getSize();
	}



	void loadFromTXT(std::string name_of_file, std::vector < std::vector < NotColor> >& matrix) {
		std::ifstream pixels;
		pixels.open(name_of_file);

		if (!pixels.is_open()) {
			std::cout << "Can't open file";
			return;
		}
		int r, g, b, a;
		for (int i = 0; i < size_of_pic.x; i++) {
			for (int j = 0; j < size_of_pic.y; j++) {
				{
					pixels >> r >> g >> b >> a;
					matrix[i][j].r = r;
					matrix[i][j].g = g;
					matrix[i][j].b = b;
					matrix[i][j].a = a;
				}
			}
		}
	}
};

void krug(const int count_of_points, const int R, const sf::Vector2i center) {																//делает пустую окружность из линий
	const double ang_step = 360 / count_of_points;												//шаг в градусах между точками
		//создаем массив точек, по которым будут рисоваться линии:


	//std::cout << "!!!";
	for (int i = 0; i < count_of_points; i++) {
		lines[i].position = sf::Vector2f(center.x + R * cos(i * ang_step * pi / 180), center.y + R * sin(i * ang_step * pi / 180));
		lines[i].color = sf::Color::Black;
	}

	//и в конце выводим все на экран:

}

void setPixelColor(sf::Texture& texture, std::vector<Pixel> pixels) {
	// Get texture image
	sf::Image image = texture.copyToImage();

	for (int i = 0; i < pixels.size(); i++) {
		// Set pixel to color
		image.setPixel(pixels[i].x, pixels[i].y, pixels[i].color);
	}

	// Update texture
	texture.loadFromImage(image);

}



int main() {
	//sf::RenderWindow screen(sf::VideoMode(900, 600), "window");
	Background back(WAY_TO_IMAGE);
	std::cout << "Did you add new picture? (Y/N)" << std::endl;
	char answer;
	while (1) {
		std::cin >> answer;
		if (answer == 'Y') {
			std::cout << "Wait, please" << std::endl;
			back.prepare(WAY_TO_IMAGE, WAY_TO_TXT);
			break;
		}
		if (answer == 'N') {
			break;
		}
		std::cout << "Try again" << std::endl;
	}

	Lens test_lens;
	test_lens.SetObjectDistance(75);
	test_lens.SetNLens(1.5);
	test_lens.SetFocus(30);
	//std::cout << test_lens.GetObjectDistance();
	Point test_point(test_lens.GetObjectDistance(), 20);

	sf::Sprite background;
	sf::Texture texture, def_texture;
	int n, m;
	n = back.getPicSize().x;
	m = back.getPicSize().y;
	std::vector < std::vector < NotColor > > matrix(n, std::vector <NotColor>(m));
	std::vector <Pixel> objects;
	back.loadFromTXT(WAY_TO_TXT, matrix);


	if (!texture.loadFromFile(WAY_TO_IMAGE)) {
		std::cout << "Error loading texture" << std::endl;
	}
	def_texture.loadFromFile(WAY_TO_IMAGE);
	background.setTexture(texture);

	std::vector<Pixel> pixels;
	Pixel pixel;


	while (screen.isOpen()) {
		sf::Event event;
		while (screen.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
			{
				screen.close();
				return -1;
			}
			case sf::Event::MouseButtonPressed:
			{
				texture = def_texture;
				krug(360, R, { event.mouseButton.x , event.mouseButton.y });
				pixelConvert(R, { event.mouseButton.x , event.mouseButton.y }, pixels, matrix, { n,m }, test_lens, test_point);

				/*pixelConvert(10, { event.mouseButton.x , event.mouseButton.y },
					objects, lens,
					matrix);*/

					//for (int i = 0; i < objects.size(); i++) {
					//	//std::cout << objects[i].getPosition().x << " " << objects[i].getPosition().y << std::endl < ;
					//}
			}

			}
		}

		clock_t start, end;
		start = clock();


		end = clock();

		setPixelColor(texture, pixels);

		pixels.clear();
		//printf("The above code block was executed in %.4f second(s)\n", ((double)end - start) / ((double)CLOCKS_PER_SEC));
		screen.clear(sf::Color::White);
		screen.draw(background);
		screen.draw(lines);
		screen.display();

	}
}
