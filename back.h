//
// Created by Student on 29.11.2024.
//

#ifndef BACK_H
#define BACK_H
#include <iostream>
#include <vector>
#include <cmath>
using namespace std;
#define M_PI 3.141592653589793238462643383279502884

using namespace std;

const double min_latitude = 48.980;
const double max_latitude = 48.990;
const double min_longitude = 8.390;
const double max_longitude = 8.410;


struct coordinates {
    double latitude;
    double longitude;
    void print(int i) const {
        cout << "Coordinates from the " << i + 1 << " point are:\n";
        cout << "latitude: " << latitude << "; longitude: " << longitude << "\n";
    }
    coordinates(double lat, double lon) :
        latitude(lat), longitude(lon) {}
};

coordinates generate_random_point() {
    double lat = max_latitude - double(rand()) / RAND_MAX * (max_latitude - min_latitude);
    double lon = max_longitude - double(rand()) / RAND_MAX * (max_longitude - min_longitude);
    return {lat, lon};
}

vector<coordinates> generate_possible_points(int count) {
    vector<coordinates> points;
    for (int i = 0; i < count; i++) {
        points.push_back(generate_random_point());
    }
    return points;
}
double is_obstacle_on_line(const coordinates& start, const coordinates& end, const coordinates& obstacle) {
    // Вычисляем расстояние от точки до линии
    double A = obstacle.latitude - start.latitude;
    double B = obstacle.longitude - start.longitude;
    double C = end.latitude - start.latitude;
    double D = end.longitude - start.longitude;

    double dot = A * C + B * D;
    double len_sq = C * C + D * D;
    double param = (len_sq != 0) ? (dot / len_sq) : -1;

    double xx, yy;
    if (param < 0) {
        xx = start.latitude;
        yy = start.longitude;
    } else if (param > 1) {
        xx = end.latitude;
        yy = end.longitude;
    } else {
        xx = start.latitude + param * C;
        yy = start.longitude + param * D;
    }

    double dx = obstacle.latitude - xx;
    double dy = obstacle.longitude - yy;
    return std::sqrt(dx * dx + dy * dy);
}

static double to_radians(double degree) {
    return degree / 180 * M_PI;
}

static double gps_distance(coordinates start, coordinates end) {
    double lat1_rad = to_radians(start.latitude);
    double lat2_rad = to_radians(end.latitude);
    double lon1_rad = to_radians(start.longitude);
    double lon2_rad = to_radians(end.longitude);

    double d_lat = lat2_rad - lat1_rad;
    double d_lon = lon2_rad - lon1_rad;

    double a = sin(d_lat / 2) * sin(d_lat / 2) +
        cos(lat1_rad) * cos(lat2_rad) * sin(d_lon / 2) * sin(d_lon / 2);

    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double distance = c * 6371000;
    return distance;
}

vector<coordinates> generate_obstacles(int count, const vector<coordinates>& reserved_points) {
    vector<coordinates> obstacles;

    while (obstacles.size() < count) {
        coordinates condidate = generate_random_point();
        bool valid = true;
        for (const auto reserved_point : reserved_points) {
            if (gps_distance(condidate, reserved_point) < 5)
            {
                valid = false;
                break;
            }
        }

        if (valid)
        {
            obstacles.push_back(condidate);
        }
    }
    return obstacles;
}

vector<coordinates> interim_coo(coordinates start, coordinates end, int steps) {
    vector<coordinates> result;
    for (float i = 0; i <= steps; i++) {
        double latitude_step = start.latitude + i / steps * (end.latitude - start.latitude);
        double longitude_step = start.longitude + i / steps * (end.longitude - start.longitude);
        result.push_back({ latitude_step, longitude_step });
    }
    return result;
}

bool is_route_valid(const std::vector<coordinates>& route, const std::vector<coordinates>& obstacles) {
    auto point_to_line_distance = [](coordinates a, coordinates b, coordinates p) {
        double A = p.latitude - a.latitude;
        double B = p.longitude - a.longitude;
        double C = b.latitude - a.latitude;
        double D = b.longitude - a.longitude;

        double dot = A * C + B * D;
        double len_sq = C * C + D * D;
        double param = (len_sq != 0) ? (dot / len_sq) : -1;

        double xx, yy;
        if (param < 0) {
            xx = a.latitude;
            yy = a.longitude;
        } else if (param > 1) {
            xx = b.latitude;
            yy = b.longitude;
        } else {
            xx = a.latitude + param * C;
            yy = a.longitude + param * D;
        }

        double dx = p.latitude - xx;
        double dy = p.longitude - yy;
        return std::sqrt(dx * dx + dy * dy);
    };

    for (size_t i = 0; i < route.size() - 1; ++i) {
        for (const auto& obstacle : obstacles) {
            double distance = point_to_line_distance(route[i], route[i + 1], obstacle);
            if (distance < 0.002) {
                return false;
            }
        }
    }
    return true;
}


std::vector<coordinates> reroute(const coordinates& start, const coordinates& end, const std::vector<coordinates>& obstacles) {
    std::vector<coordinates> new_route = interim_coo(start, end, 5);

    for (size_t i = 0; i < new_route.size() - 1; ++i) {
        for (const auto& obstacle : obstacles) {
            while (true) {
                double distance = is_obstacle_on_line(new_route[i], new_route[i + 1], obstacle);

                if (distance < 0.002) {
                    double offset_lat = (new_route[i].latitude + new_route[i + 1].latitude) / 2 - obstacle.latitude;
                    double offset_lon = (new_route[i].longitude + new_route[i + 1].longitude) / 2 - obstacle.longitude;

                    new_route[i].latitude += offset_lat * 0.1;
                    new_route[i].longitude += offset_lon * 0.1;

                    new_route[i + 1].latitude += offset_lat * 0.1;
                    new_route[i + 1].longitude += offset_lon * 0.1;
                } else {
                    break;
                }
            }
        }
    }
    return new_route;
}


#endif //BACK_H
