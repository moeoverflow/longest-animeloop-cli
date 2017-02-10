//
//  main.cpp
//  loop
//
//  Created by BlueCocoa on 2017/2/10.
//  Copyright © 2017年 BlueCocoa. All rights reserved.
//

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <sstream>

class loop_frame {
public:
    loop_frame(std::string hash, int index) : _hash(hash), _index(index) {
    }
    
    int index() const {
        return _index;
    }
    
    std::string hash() const {
        return _hash;
    }
private:
    int _index;
    std::string _hash;
};

void loop(std::string file, double max_loop_time, double min_loop_time, int step, std::string clips) {
    cv::VideoCapture video(file);
    
    if (!video.isOpened()) {
        return;
    }
    
    size_t frame_count = video.get(CV_CAP_PROP_FRAME_COUNT);
    int fps = video.get(CV_CAP_PROP_FPS);
    
    if (frame_count <= fps * min_loop_time) {
        return;
    }
    
    int max_loop_frame_count = max_loop_time * fps;
    int min_loop_frame_count = min_loop_time * fps;
    size_t ste = (min_loop_frame_count / (step + 1));
    
    size_t extracted = 0;
    
    // dHash function
    // copied and paste from Shincurry
    auto dHash = [](cv::Mat& image, cv::Size size) {
        cv::resize(image, image, size);
        cv::cvtColor(image, image, CV_RGB2GRAY);
        
        std::string difference;
        
        for (int row = 0; row < size.height; ++row) {
            int row_start_index = row * size.height;
            for (int col = 0; col < size.width - 1; ++col) {
                int col_left_index = row_start_index + col;
                bool diff = image.at<uchar>(col_left_index) > image.at<uchar>(col_left_index+1);
                difference.append(diff ? "1" : "0");
            }
        }
        return difference;
    };
    
    auto fill_to = [&extracted, dHash](cv::VideoCapture & video, std::vector<loop_frame> & frames, size_t count, int step, cv::Size size) {
        cv::Mat frame;
        int emplaced = 0;
        
        count = (count - frames.size()) / (step + 1);
        while (emplaced++ < count) {
            if (!video.read(frame)) break;
            frames.emplace_back(dHash(frame, size), extracted);
            extracted++;
            int _step = 0;
            while (_step++ < step && video.read(frame)) {
                extracted++;
            }
        }
    };
    
    // hamming distance function
    // copied and paste from Shincurry
    auto hamming_distance = [](std::string str1, std::string str2) -> unsigned int {
        if (str1.empty() || str2.empty()) {
            return 0;
        }
        
        unsigned long len1 = str1.length();
        unsigned long len2 = str2.length();
        
        if (len1 != len2) {
            return 0;
        }
        
        unsigned int dist = 0;
        for (int i = 0; i < len1; ++i) {
            dist += (str1.at(i) != str2.at(i)) ? 1 : 0;
        }
        return dist;
    };
    
    std::vector<loop_frame> frames;
    std::vector<std::pair<int, int>> loops;
    
    // search starts from the end to make it as long as possible
    while (extracted != frame_count) {
        fill_to(video, frames, max_loop_frame_count, step, cv::Size(8, 9));
        bool found = false;
//        std::cout << "buffer " << frames.begin()->index() << " - " << (frames.end() - 1)->index() << '\n';
        for (auto it = frames.begin(); (frames.end() - 1)->index() - it->index() >= min_loop_frame_count; it++) {
            auto end = frames.end();
            end--;
//            std::cout << "compare " << it->index() << " - " << end->index() << '\n';
            
            std::vector<int> distances;
            std::vector<int> nearbyDistances;
            for (; end->index() - it->index() <= max_loop_frame_count && end->index() - it->index() >= min_loop_frame_count; end--) {
//                std::cout << "compare " << it->index() << " - " << end->index() << '\n';
                int distance = hamming_distance(end->hash(), it->hash());
                distances.emplace_back(distance);
                if (distance == 0) {
//                    std::cout << it->index() << " - " << end->index() << '\n';
                    loops.emplace_back(it->index(), end->index());
                    found = true;
                    break;
                }
            }
            
            if (found) {
                frames.erase(frames.begin(), end);
                break;
            }
        }
        if (!found) {
            size_t erase = (frames.size() < ste ? frames.size() / 3 : ste);
            frames.erase(frames.begin(), frames.begin() + (erase == 0 ? 1 : erase));
        }
    }
    
    std::cout << loops.size() << " loops\n";
    std::for_each(loops.begin(), loops.end(), [&video, &clips, fps](const std::pair<int, int>& loop) {
        std::cout << loop.first << " - " << loop.second << '\n';
        
        video.set(CV_CAP_PROP_POS_FRAMES, loop.first);
        std::ostringstream filename;
        
        // where clips goes
        filename << clips << "/" << double(loop.first * 1.0 / fps)  << " - " << double(loop.second * 1.0 / fps) << ".mp4";
        
        cv::VideoWriter writer(filename.str(), video.get(CV_CAP_PROP_FOURCC), fps, cv::Size(video.get(CV_CAP_PROP_FRAME_WIDTH), video.get(CV_CAP_PROP_FRAME_HEIGHT)));
        
        cv::Mat image;
        int frame = loop.first;
        while (video.read(image)) {
            if (frame > loop.second) {
                break;
            }
            writer.write(image);
            frame++;
        }
    });
}
int main(int argc , const char * argv[]) {
    if (argc != 2 && argc != 3) {
        std::cout << "Usage: loop [video file] [clips directory]\n";
        exit(0);
    }
    std::string file = std::string(argv[1]);
    std::string clips = "~";
    if (argc == 3) {
        clips = std::string(argv[2]);
        if (clips.at(clips.size()) == '/') {
            std::cout << "Error: please do not type trailing '/'\n";
            exit(0);
        }
    }
    // please using standardized path or relative path
    
    double max_loop_time = 5.0;
    double min_loop_time = 0.9;
    int step = 0;
    
    loop(file, max_loop_time, min_loop_time, step, clips);

    return 0;
}
