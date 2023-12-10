#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <cmath>
#include <algorithm>
#include <clocale>
#include <optional>
#include <list>

struct pixel{
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

struct range{
	int start;
	int end;
};

pixel get_pixel(std::vector<uint8_t>& data, size_t width, size_t height, size_t col, size_t row) {
	pixel p;
	p.r = data[3 * (row * width + col) + 0];
	p.g = data[3 * (row * width + col) + 1];
	p.b = data[3 * (row * width + col) + 2];
	return p;
}

int64_t pixel_color_distance2(const pixel& p1, const pixel& p2) {
	return (p1.r - p2.r) * (p1.r - p2.r) + (p1.g - p2.g) * (p1.g - p2.g) + (p1.b - p2.b) * (p1.b - p2.b);
}

double pixel_color_distance(const pixel& p1, const pixel& p2) {
	return std::sqrt(double(pixel_color_distance2(p1, p2)));
}

size_t pixel_color_to_population(const std::vector<pixel>& reference_colors, const std::vector<range>& population_densities, const pixel& p) {
	// int64_t distanse2 = pixel_color_distance2(reference_colors[0], p);
	double distanse = pixel_color_distance(reference_colors[0], p);
	size_t index = 0;
	for(size_t i = 1; i < reference_colors.size(); i++) {
		// int64_t tmp_distanse2 = pixel_color_distance2(reference_colors[i], p);
		double tmp_distanse = pixel_color_distance(reference_colors[i], p);
		if(tmp_distanse < distanse) {
			distanse = tmp_distanse;
			index = i;
		}
	}
	return (population_densities[index].start + population_densities[index].end) / 200;
}

void extract_population_data() {
    std::string filename1{"../data/Screenshot_20231122_215705.ppm"};
    std::string filename2{"../data/city.ppm"};
    std::fstream s1{filename1, s1.binary | s1.in};
    std::fstream s2{filename2, s2.binary | s2.out};
 
    if (!s1.is_open() or !s2.is_open())
        std::cout << "failed to open " << '\n';
    else
    {
		std::string str;
		std::cout << "Зчитуємо зображення для густини населення: " << filename1 << "\n";
		for(int i = 0; i < 4; i++) {
			s1 >> str;
			std::cout << "\t" << str << "\n";
		}
		char c;
		s1 >> std::ws;
		std::vector<uint8_t> binaryData;
		binaryData.insert(	binaryData.begin(),
			std::istreambuf_iterator<char>(s1),
			std::istreambuf_iterator<char>()
		);
		std::cout << "Кількість пікселей:\n";
		std::cout << "\t" << binaryData.size() << " - очікувана\n";
		std::cout << "\t" << 1920 * 1080 * 3 << " - дійсна\n";
		// std::cout << "6 перших пікселей: ";
		// std::cout << int(binaryData[0]) << " ";
		// std::cout << int(binaryData[1]) << " ";
		// std::cout << int(binaryData[2]) << " ";
		// std::cout << int(binaryData[3]) << " ";
		// std::cout << int(binaryData[4]) << " ";
		// std::cout << int(binaryData[5]) << " ";
		// std::cout << "\n";
		
		size_t city_width = 300;
		size_t city_height = 250;
		size_t city_offset_x = 748;
		size_t city_offset_y = 543;

		std::vector<pixel> cityData(city_height * city_width);
		for(int i = 0; i < city_height; i++) {
			for(int j = 0; j < city_width; j++) {
				cityData[(i * city_width + j)].r = binaryData[3 * ((i + city_offset_y) * 1920 + (j + city_offset_x)) + 0];
				cityData[(i * city_width + j)].g = binaryData[3 * ((i + city_offset_y) * 1920 + (j + city_offset_x)) + 1];
				cityData[(i * city_width + j)].b = binaryData[3 * ((i + city_offset_y) * 1920 + (j + city_offset_x)) + 2];
			}
		}
		
		std::cout << "Зріз міста Київ: " << city_width << "x" << city_height << " пікселей\n";
		std::cout << "\t" << city_offset_x << " - зсув по x\n";
		std::cout << "\t" << city_offset_y << " - зсув по y\n";
		
		s2 << "P6\n" << city_width << " " << city_height << "\n" << 255 << "\n";
		for(int i = 0; i < cityData.size(); ++i) {
			s2 << cityData[i].r;
			s2 << cityData[i].g;
			s2 << cityData[i].b;
		}
		s2.close();
		std::cout << "\tзбережено у файл: " << filename2 << "\n";
		
		std::vector<pixel> reference_colors(16);
		reference_colors[0] = get_pixel(binaryData, 1920, 1080, 70, 185); // 20-99
		reference_colors[1] = get_pixel(binaryData, 1920, 1080, 70, 200); // 100-399
		reference_colors[2] = get_pixel(binaryData, 1920, 1080, 130, 185); // 400-1k
		reference_colors[3] = get_pixel(binaryData, 1920, 1080, 130, 200); // 1k-2k
		reference_colors[4] = get_pixel(binaryData, 1920, 1080, 190, 185); // 2k-3.5k
		reference_colors[5] = get_pixel(binaryData, 1920, 1080, 190, 200); // 3.5k-5.5k
		reference_colors[6] = get_pixel(binaryData, 1920, 1080, 250, 185); // 5.5k-7.5k
		reference_colors[7] = get_pixel(binaryData, 1920, 1080, 250, 200); // 7.5k-10k
		reference_colors[8] = get_pixel(binaryData, 1920, 1080, 315, 185); // 10k-12k
		reference_colors[9] = get_pixel(binaryData, 1920, 1080, 315, 200); // 12k-16k
		reference_colors[10] = get_pixel(binaryData, 1920, 1080, 375, 185); // 16k-22k
		reference_colors[11] = get_pixel(binaryData, 1920, 1080, 375, 200); // 22k-30k
		reference_colors[12] = get_pixel(binaryData, 1920, 1080, 435, 185); // 30k-50k
		reference_colors[13] = get_pixel(binaryData, 1920, 1080, 435, 200); // 50k-100k
		reference_colors[14] = get_pixel(binaryData, 1920, 1080, 495, 185); // 100k-200k
		reference_colors[15] = get_pixel(binaryData, 1920, 1080, 495, 200); // 200+k
		std::vector<range> population_densities(16);
		population_densities[0] = {20, 99};
		population_densities[1] = {100, 399};
		population_densities[2] = {400, 999};
		population_densities[3] = {1000, 1999};
		population_densities[4] = {2000, 3499};
		population_densities[5] = {3500, 5499};
		population_densities[6] = {5500, 7499};
		population_densities[7] = {7500, 9999};
		population_densities[8] = {10000, 11999};
		population_densities[9] = {12000, 15999};
		population_densities[10] = {16000, 21999};
		population_densities[11] = {22000, 29999};
		population_densities[12] = {30000, 49999};
		population_densities[13] = {50000, 99999};
		population_densities[14] = {100000, 199999};
		population_densities[15] = {200000, 399999};
		// for(size_t i = 108; i < 190; ++i) {
		// 	std::cout << i << ":" << pixel_color_to_population(reference_colors, population_densities, cityData[81 * city_width + i]) << "\n";
		// }
		size_t total_population = 0;
		for(auto p : cityData) {
			total_population += pixel_color_to_population(reference_colors, population_densities, p);
		}
		std::cout << "Розрахунок кількості населення за кольорами на зображенні...\n";
		std::cout << "Сумарна кількість населення для Києва: " << total_population << "\n";
		
		std::string filename3{"../data/consumers_km.txt"};
		std::fstream s3{filename3, s3.out};
		 
		if (!s3.is_open())
			std::cout << "failed to open " << filename3 << '\n';
		else
		{
			for(size_t i = 0; i < cityData.size(); i++)
			{
				const auto p = cityData[i];
				size_t col = i % city_width;
				size_t row = i / city_width;
				double km_per_pixel = 0.1;
				double x = km_per_pixel * col;
				double y = km_per_pixel * (city_height - row);
				s3 << x << " " << y << " " << pixel_color_to_population(reference_colors, population_densities, p) << "\n";
			}
		}
		std::cout << "Дані по населенню збережені у файл: " << filename3 << "\n";
	}
}

std::pair<double, double> gps_to_xy_km(double latitude, double longitude) {
	const double latitude_offset = 50.4604093;
	const double longitude_offset = 30.5252086;
	const double latitude_scale = 111.0; // кілометр на градус
	const double longitude_scale = 71.7; // кілометр на градус
	const double x_offeset = 15.5; // кілометрів від лівого нижнього края зрізу
	const double y_offeset = 15.8; // кілометрів від лівого нижнього края зрізу
	std::pair<double, double> xy;
	xy.first = x_offeset + longitude_scale * (longitude - longitude_offset);
	xy.second = y_offeset + latitude_scale * (latitude - latitude_offset);
	return xy;
}

void extract_locations_data() {
    std::string filename1{"../data/locations_gps.txt"};
    std::string filename2{"../data/locations_km.txt"};
    std::fstream s1{filename1, s1.binary | s1.in};
    std::fstream s2{filename2, s2.binary | s2.out};
 
    if (!s1.is_open() or !s2.is_open())
        std::cout << "failed to open " << filename1 << " or " << filename2 << '\n';
    else
    {
		size_t i = 0;
		std::string tmp1 = "";
		while(getline(s1, tmp1)) {
			++i;
			std::string tmp2 (std::find(tmp1.begin(), tmp1.end(), '@') + 1, std::end(tmp1));
			std::stringstream ss2(tmp2);
			double latitude, longitude;
			double rent_usd;
			char discard_c;
			std::string discard_str;
			ss2 >> latitude;
			ss2 >> discard_c; // Ігноруємо кому
			ss2 >> longitude;
			ss2 >> discard_str; // Ігноруємо залишок строки з координатою z
			ss2 >> discard_c; // Ігноруємо символ "$"
			ss2 >> rent_usd;
			double rent_hrn = 37.4 * rent_usd;
			const auto xy = gps_to_xy_km(latitude, longitude);
			s2 << xy.first << " " << xy.second << " " << rent_hrn << "\n";
		}
		std::cout << "Кількість оброблених локацій: " << i << "\n";
	}
}

void extract_competitors_data() {
    std::string filename1{"../data/competitors_gps.txt"};
    std::string filename2{"../data/competitors_km.txt"};
    std::fstream s1{filename1, s1.binary | s1.in};
    std::fstream s2{filename2, s2.binary | s2.out};
	std::cout << "Розрахунок даних конкурентів:\n";
	std::cout << "\t" << filename1 << " - вхідні дані\n";
	std::cout << "\t" << filename2 << " - вихідні дані\n";
    if (!s1.is_open() or !s2.is_open())
        std::cout << "failed to open " << filename1 << " or " << filename2 << '\n';
    else
    {
		size_t i = 0;
		std::string tmp1 = "";
		while(getline(s1, tmp1)) {
			++i;
			std::string tmp2 (std::find(tmp1.begin(), tmp1.end(), '@') + 1, std::end(tmp1));
			std::stringstream ss2(tmp2);
			double latitude, longitude;
			char discard_c;
			std::string discard_str;
			ss2 >> latitude;
			ss2 >> discard_c; // Ігноруємо кому
			ss2 >> longitude;
			const auto xy = gps_to_xy_km(latitude, longitude);
			// s2 << latitude << " " << longitude << "\n";
			s2 << xy.first << " " << xy.second << "\n";
		}
		std::cout << u8"Кількість оброблених конкурентів: " << i << "\n";
	}
}

void extract_productions_data() {
    std::string filename1{"../data/productions_gps.txt"};
    std::string filename2{"../data/productions_km.txt"};
    std::fstream s1{filename1, s1.binary | s1.in};
    std::fstream s2{filename2, s2.binary | s2.out};
 
    if (!s1.is_open() or !s2.is_open())
        std::cout << "failed to open " << filename1 << " or " << filename2 << '\n';
    else
    {
		size_t i = 0;
		std::string tmp1 = "";
		while(getline(s1, tmp1)) {
			++i;
			std::string tmp2 (std::find(tmp1.begin(), tmp1.end(), '@') + 1, std::end(tmp1));
			std::stringstream ss2(tmp2);
			double latitude, longitude;
			char discard_c;
			std::string discard_str;
			ss2 >> latitude;
			ss2 >> discard_c; // Ігноруємо кому
			ss2 >> longitude;
			const auto xy = gps_to_xy_km(latitude, longitude);
			// s2 << latitude << " " << longitude << "\n";
			s2 << xy.first << " " << xy.second << "\n";
		}
		std::cout << u8"Кількість оброблених потужностей (млинів): " << i << "\n";
	}
}

struct consumer{
	double x;
	double y;
	int64_t count;
};

struct location{
	double x;
	double y;
	double rent;
};

struct competitor{
	double x;
	double y;
};

struct production{
	double x;
	double y;
};

double distanse2(location xi, competitor yj) {
	return (xi.x - yj.x) * (xi.x - yj.x) + (xi.y - yj.y) * (xi.y - yj.y);
}

double distanse(location xi, competitor yj) {
	return sqrt((xi.x - yj.x) * (xi.x - yj.x) + (xi.y - yj.y) * (xi.y - yj.y));
}

double distanse2(location xi, consumer yj) {
	return (xi.x - yj.x) * (xi.x - yj.x) + (xi.y - yj.y) * (xi.y - yj.y);
}

double distanse(location xi, consumer yj) {
	return sqrt((xi.x - yj.x) * (xi.x - yj.x) + (xi.y - yj.y) * (xi.y - yj.y));
}

double distanse(location xi, production yj) {
	return sqrt((xi.x - yj.x) * (xi.x - yj.x) + (xi.y - yj.y) * (xi.y - yj.y));
}

std::vector<size_t> solve_tz(const std::vector<double>& cs, const std::vector<size_t>& a, const std::vector<size_t>& b) {
	std::vector<size_t> xs;
	xs.resize(a.size() * b.size());
	// sum a = sum b - збалансова транспортна задача
	{ // Пошук розв'язку транспортної задачі для xs
		// { // Тривіальний план
		// 	for(size_t i = 0; i < a.size(); ++i) {
		// 		for(size_t j = 0; j < b.size(); ++j) {
		// 			xs[i * b.size() + j] = 0;
		// 		}
		// 	}
		// }

		std::vector<size_t> tz_plan;
		{ // ПЗК
			for(size_t i = 0; i < a.size(); ++i) {
				for(size_t j = 0; j < b.size(); ++j) {
					xs[i * b.size() + j] = 0;
				}
			}
			auto cons = a[0];
			auto prod = b[0];
			for(size_t i = 0, j = 0; (i < a.size()) && (j < b.size()); ) {
				bool cons_min = cons == std::min(cons, prod);
				bool prod_min = prod == std::min(cons, prod);
				if(!cons_min && !prod_min) throw(std::exception());
				xs[i * b.size() + j] = std::min(cons, prod);
				tz_plan.push_back(i * b.size() + j);
				if(cons_min) {
					prod -= xs[i * b.size() + j];
					i++;
					cons = a[i];
				} else if(prod_min) {
					cons -= xs[i * b.size() + j];
					j++;
					prod = b[j];
				}
			}
		}
		
		{ // Метод потенціалів
			size_t potential_method_iteration = 0;
			while(true) {
				potential_method_iteration++;
				std::vector<std::optional<double>> u(a.size(), std::nullopt);
				std::vector<std::optional<double>> v(b.size(), std::nullopt);
				{ // Розрахунок потенціалів
					u[0] = 0.0;
					while(true) {
						bool tmp = true;
						for(auto u0 : u) {
							if(u0 == std::nullopt) {
								tmp = false;
							}
						}
						for(auto v0 : v) {
							if(v0 == std::nullopt) {
								tmp = false;
							}
						}
						if(tmp) break;
						for(size_t ij : tz_plan) {
							size_t i = ij / b.size();
							size_t j = ij % b.size();
							if((u[i] == std::nullopt) xor ((v[j] == std::nullopt))) {
								if(u[i] == std::nullopt) {
									u[i] = cs[ij] - v[j].value();
								}
								if(v[j] == std::nullopt) {
									v[j] = cs[ij] - u[i].value();
								}
							}
						}
					}
				}
				
				std::vector<double> cs_eff(cs.size());
				{ // Розрахунок псевдовартостей
					for(size_t i = 0; i < u.size(); ++i) {
						for(size_t j = 0; j < v.size(); ++j) {
							cs_eff[i * v.size() + j] = u[i].value() + v[j].value();
						}
					}
				}
					
				size_t max_diff_ij = 0;
				for(size_t ij = 0; ij < cs.size(); ++ij) {
					if(cs_eff[ij] - cs[ij] > cs_eff[max_diff_ij] - cs[max_diff_ij]) {
						max_diff_ij = ij;
					}
				}

				{ // Вивести поточні розрахунки
					// for(size_t i = 0; i < u.size(); ++i) {
					// 	for(size_t j = 0; j < v.size(); ++j) {
					// 		const size_t ij = i * v.size() + j;
					// 		std::cout << xs[ij] << " ";
					// 	}
					// 	std::cout << "\n";
					// }
					// for(size_t i = 0; i < u.size(); ++i) {
					// 	for(size_t j = 0; j < v.size(); ++j) {
					// 		const size_t ij = i * v.size() + j;
					// 		std::cout << cs[ij] << " ";
					// 	}
					// 	std::cout << "\n";
					// }
					// for(size_t i = 0; i < u.size(); ++i) {
					// 	for(size_t j = 0; j < v.size(); ++j) {
					// 		const size_t ij = i * v.size() + j;
					// 		std::cout << cs_eff[ij] << " ";
					// 	}
					// 	std::cout << "\n";
					// }
				}
				if(cs_eff[max_diff_ij] <= cs[max_diff_ij])
					break;
				
				std::vector<size_t> loop_ijs;
				{ // Алгоритм методу викреслювання для знаходження циклу
					std::list<size_t> loop_is;
					std::list<size_t> loop_js;
					for(size_t i = 0; i < u.size(); ++i) {
						loop_is.push_back(i);
					}
					for(size_t j = 0; j < v.size(); ++j) {
						loop_js.push_back(j);
					}
					
					while(true) {
						bool loop_ids_changed = false;
						for (auto it_is = loop_is.begin(); it_is != loop_is.end();)
						{
							if ([&](size_t i) {
								size_t tmp = 0;
								for(auto j : loop_js) {
									const size_t ij = i * v.size() + j;
									if((ij == max_diff_ij) or (xs[ij] != 0.0)) {
										tmp += 1;
									}
								}
								return tmp <= 1;
							} (*it_is)) {
								it_is = loop_is.erase(it_is);
								loop_ids_changed = true;
							}
							else
								++it_is;
						}
						for (auto it_js = loop_js.begin(); it_js != loop_js.end();)
						{
							if ([&](size_t j) {
								size_t tmp = 0;
								for(auto i : loop_is) {
									const size_t ij = i * v.size() + j;
									if((ij == max_diff_ij) or (xs[ij] != 0.0)) {
										tmp += 1;
									}
								}
								return tmp <= 1;
							} (*it_js)) {
								it_js = loop_js.erase(it_js);
								loop_ids_changed = true;
							}
							else
								++it_js;
						}
						if(!loop_ids_changed)
							break;
					}
					loop_ijs.push_back(max_diff_ij);
					size_t tmp_ij = max_diff_ij;
					for(size_t k = 0; k < loop_is.size(); ++k) {
						{ // Рух по вертикалі
							size_t i = tmp_ij / v.size();
							for(auto j : loop_js) {
								const size_t ij = i * v.size() + j;
								if((xs[ij] != 0.0) and tmp_ij != ij) {
									loop_ijs.push_back(ij);
									tmp_ij = ij;
									break;
								}
							}
						}
						{ // Рух по горизонталі
							size_t j = tmp_ij % v.size();
							for(auto i : loop_is) {
								const size_t ij = i * v.size() + j;
								if((xs[ij] != 0.0) and tmp_ij != ij) {
									loop_ijs.push_back(ij);
									tmp_ij = ij;
									break;
								}
							}
						}
					}
				}

				//std::cout << "max_diff: " << cs_eff[max_diff_ij] - cs[max_diff_ij] << "\n";
				//for(auto ij : loop_ijs) {
					//std::cout << xs[ij] << " ";
				//}
				//std::cout << "\n";
				{ // Формування нового опорного плану з циклу
					auto min_odd = xs[loop_ijs[1]];
					for(size_t k = 2; k < loop_ijs.size(); k += 2) {
						if(xs[loop_ijs[k + 1]] < min_odd) {
							min_odd = xs[loop_ijs[k + 1]];
						}
					}
					for(size_t k = 0; k < loop_ijs.size(); k += 2) {
						xs[loop_ijs[k]] += min_odd;
						xs[loop_ijs[k + 1]] -= min_odd;
					}
					tz_plan.clear();
					for(size_t ij = 0; ij < xs.size(); ++ij) {
						if(xs[ij] > 0)
							tz_plan.push_back(ij);
					}
				}
				//for(auto ij : loop_ijs) {
					//std::cout << xs[ij] << " ";
				//}
				//std::cout << "\n";
				// if(potential_method_iteration > 10) {
				// 	break;
				// }
			} // while
		} // Метод потенціалів
	} // транспортна задача
	return xs;
}

int main() {
	std::setlocale(LC_CTYPE, "en_US.UTF-8");
    extract_population_data(); // Викликати лише один раз, щоб згенерувати данні по населенню
	extract_locations_data(); // Викликати лише один раз, щоб перевести дані локацій з GPS у метричні координати
	extract_competitors_data(); // Викликати лише один раз, щоб перевести дані конкурентів з GPS у метричні координати
	extract_productions_data(); // Викликати лише один раз, щоб перевести дані потужностей виробництва з GPS у метричні координати

	std::cout << u8"ЕТАП 1.\n";
	std::vector<consumer> consumers;
	{
		std::string filename1{"../data/consumers_km.txt"};
		std::fstream s1{filename1, s1.binary | s1.in};
		if (!s1.is_open())
			std::cout << "failed to open " << filename1 << '\n';
		else
		{
			int64_t n = 0;
			std::string tmp1 = "";
			while(getline(s1, tmp1)) {
				std::stringstream ss2(tmp1);
				consumer tmp;
				ss2 >> tmp.x;
				ss2 >> tmp.y;
				ss2 >> tmp.count;
				consumers.push_back(tmp);
				n += tmp.count;
			}
			std::cout << u8"Зчитано точок для споживачів: " << consumers.size() << "\n";
			std::cout << u8"Всього людей у місті: " << n << "\n";
		}
	}
	std::vector<location> locations;
	{
		std::string filename1{"../data/locations_km.txt"};
		std::fstream s1{filename1, s1.binary | s1.in};
		if (!s1.is_open())
			std::cout << "failed to open " << filename1 << '\n';
		else
		{
			std::string tmp1 = "";
			while(getline(s1, tmp1)) {
				std::stringstream ss2(tmp1);
				location tmp;
				ss2 >> tmp.x;
				ss2 >> tmp.y;
				ss2 >> tmp.rent;
				locations.push_back(tmp);
			}
			std::cout << u8"Зчитано потенційних локацій торгових точок: " << locations.size() << "\n";
		}
	}
	std::vector<competitor> competitors;
	{
		std::string filename1{"../data/competitors_km.txt"};
		std::fstream s1{filename1, s1.binary | s1.in};
		if (!s1.is_open())
			std::cout << "failed to open " << filename1 << '\n';
		else
		{
			std::string tmp1 = "";
			while(getline(s1, tmp1)) {
				std::stringstream ss2(tmp1);
				competitor tmp;
				ss2 >> tmp.x;
				ss2 >> tmp.y;
				competitors.push_back(tmp);
			}
			std::cout << u8"Зчитано локацій конкурентів: " << competitors.size() << "\n";
		}
	}
	std::vector<production> productions;
	{
		std::string filename1{"../data/productions_km.txt"};
		std::fstream s1{filename1, s1.binary | s1.in};
		if (!s1.is_open())
			std::cout << "failed to open " << filename1 << '\n';
		else
		{
			std::string tmp1 = "";
			while(getline(s1, tmp1)) {
				std::stringstream ss2(tmp1);
				production tmp;
				ss2 >> tmp.x;
				ss2 >> tmp.y;
				productions.push_back(tmp);
			}
			std::cout << u8"Зчитано потужностей (млинів): " << productions.size() << "\n";
		}
	}
	std::cout << "\n";
	
	std::vector<double> b;
	b.reserve(locations.size());
	for(auto xi : locations) {
		double tmpb = 0;
		for(auto yj : competitors) {
			tmpb += exp(- ((xi.x - yj.x) * (xi.x - yj.x) + (xi.y - yj.y) * (xi.y - yj.y)) / 2.0);
		}
		b.push_back(tmpb);
	}
	
	{
		std::cout << u8"Конкуренція для кожної з локацій:" << "\n";
		size_t i = 0;
		for(auto bi : b) {
			i++;
			std::cout << "\t" << i << ": " << bi + 1 << "\n";
		}
	}

	std::vector<double> a;
	a.reserve(locations.size());
	for(auto xi : locations) {
		double tmpa = 0;
		for(auto zk : consumers) {
			tmpa += 0.1 * zk.count / (distanse(xi, zk) + 0.1);
		}
		a.push_back(tmpa);
	}
	
	{
		std::cout << u8"Кількість потенційних клієнтів для кожної з локацій:" << "\n";
		size_t i = 0;
		for(auto ai : a) {
			i++;
			std::cout << "\t" << i << ": " << ai << "\n";
		}
	}

	std::vector<double> g;
	g.reserve(locations.size());
	for(size_t i = 0; i < locations.size(); i++) {
		const double c = 10; // середня кількість товару, що необхідна споживачу на місяць
		g.push_back(c * a[i] / (b[i] + 1));
	}
	
	{
		std::cout << u8"Потенційний товарообіг на місяць для кожної з локацій:" << "\n";
		size_t i = 0;
		for(auto gi : g) {
			i++;
			std::cout << "\t" << i << ": " << gi / 1000 << u8"k" << "\n";
		}
	}
	
	const double k = 10;
	std::cout << u8"Вважаємо, що націнка на товар дорівнює: " << k << "грн.\n";
	const double l = 0.5;
	
	std::cout << u8"Вважаємо, що дорожні витрати на одиницю товару на кілометр дорівнюють: " << l << "грн.\n";

	std::vector<double> p;
	p.resize(locations.size());
	{
		for(size_t i = 0; i < p.size(); ++i) {
			double s = 0.0;
			for(auto prod : productions) {
				s += distanse(locations[i], prod);
			}
			s /= productions.size();
			const double d = l * s;
			p[i] = g[i] * (k - d) - locations[i].rent;
		}
	}

	{
		std::cout << u8"Тоді розрахована прибутковість кожної з точок становитиме:\n";
		size_t i = 0;
		for(auto gi : g) {
			std::cout << "\t" << i + 1 << ": " << p[i] / 1000 << u8" тис. грн / місяць" << "\n";
			i++;
		}
	}
	
	std::vector<size_t> location_ids;
	location_ids.resize(locations.size());
	{
		std::cout << u8"Найкраща локація, та її прибутковість:\n";
		size_t maxi = 0;
		for(size_t i = 0; i < locations.size(); i++) {
			if(p[maxi] < p[i])
				maxi = i;
		}
		std::cout << "\t" << maxi + 1 << ": " << p[maxi] / 1000 << u8" тис. грн / місяць" << "\n";
		
		//
		std::cout << u8"Найкращі локації, та їх індивідуальні прибутковості у порядку спадання:\n";
		for(size_t i = 0; i < location_ids.size(); ++i) {
			location_ids[i] = i;
		}
		
		{ // Сортування локацій за прибутковістю
			std::sort(location_ids.begin(), location_ids.end(),
				[&](size_t a, size_t b) {
					return p[a] > p[b];
				}
			);
		}
		
		for(size_t i = 0; i < location_ids.size(); ++i) {
			std::cout << "\t" << location_ids[i] + 1 << ": " << p[location_ids[i]] / 1000 << u8" тис. грн / місяць" << "\n";
		}
	}

	// std::vector<double> cs;
	// cs.resize(locations.size() * productions.size());
	// {
	// 	for(size_t i = 0; i < locations.size(); ++i) {
	// 		for(size_t j = 0; j < productions.size(); ++j) {
	// 			cs[i * productions.size() + j] = distanse(locations[i], productions[j]);
	// 		}
	// 	}
	// }
	
	std::cout << "ЕТАП 2.";
	size_t n = 4;

	std::vector<size_t> chosen_locations;
	{
		chosen_locations.resize(n);
		for(size_t i = 0; i < n; ++i) {
			chosen_locations[i] = location_ids[i];
		}
		// for(size_t i = 0; i < n; ++i) {
		// 	chosen_locations[i] = location_ids[4 - i];
		// }
	}

	std::vector<double> cs;
	cs.resize(chosen_locations.size() * productions.size());
	{
		for(size_t i = 0; i < chosen_locations.size(); ++i) {
			for(size_t j = 0; j < productions.size(); ++j) {
				cs[i * productions.size() + j] = distanse(locations[chosen_locations[i]], productions[j]);
			}
		}
	}

	std::vector<size_t> xs;
	{
		std::cout << "Розрахунок транспортного плану для " << n << " торгових точок.\n";
		std::vector<size_t> a;
		a.resize(chosen_locations.size());
		for(size_t i = 0; i < a.size(); ++i)
			a[i] = productions.size();
		std::vector<size_t> b;
		b.resize(productions.size());
		for(size_t i = 0; i < b.size(); ++i)
			b[i] = chosen_locations.size();
		xs = solve_tz(cs, a, b);
	}
	{
		double msum = 0.0;
		std::cout << u8"Прибутковості точок мережі без урахування транспортної оптимізації:\n";
		for(size_t i = 0; i < chosen_locations.size(); ++i) {
			double s = 0.0;
			for(auto prod : productions) {
				s += distanse(locations[chosen_locations[i]], prod);
			}
			s /= productions.size();
			const double d = l * s;
			double mp = g[chosen_locations[i]] * (k - d) - locations[chosen_locations[i]].rent;
			std::cout << "\t" << chosen_locations[i] + 1 << ": " << mp / 1000 << u8" тис. грн / місяць" << "\n";
			msum += mp;
		}
		std::cout << "Всього: " << msum / 1000 << u8" тис. грн / місяць" << "\n";
		
		msum = 0.0;
		std::cout << u8"Прибутковості точок мережі з урахування транспортної оптимізації:\n";
		for(size_t i = 0; i < chosen_locations.size(); ++i) {
			double s = 0.0;
			for(size_t j = 0; j < productions.size(); ++j) {
				s += distanse(locations[chosen_locations[i]], productions[j]) * xs[i * productions.size() + j];
			}
			s /= productions.size();
			const double d = l * s;
			double mp = g[chosen_locations[i]] * (k - d) - locations[chosen_locations[i]].rent;
			std::cout << "\t" << chosen_locations[i] + 1 << ": " << mp / 1000 << u8" тис. грн / місяць" << "\n";
			msum += mp;
		}
		std::cout << "Всього: " << msum / 1000 << u8" тис. грн / місяць" << "\n";
	}

	return 0;
}
