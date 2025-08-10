# # 編譯器和編譯選項
# CXX = g++
# CXXFLAGS = -march=native -flto -funroll-loops -finline-functions -ffast-math -O3 -Iinc -std=c++11

# # 目標檔案和目錄
# SRC_DIR = src
# OBJ_DIR = obj
# TARGET = Lab1
# TARGET_1 = test_point_finding
# TARGET_2 = test_neighbor_finding
# TARGET_3 = test_edge_tile_finding

# # 源文件列表
# SOURCES = $(wildcard $(SRC_DIR)/*.cpp) main.cpp
# OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES:main.cpp=$(OBJ_DIR)/main.o))
# DEPS = $(OBJECTS:.o=.d)

# .PHONY: all clean test_point_finding test_neighbor_finding test_edge_tile_finding

# all: $(TARGET)

# $(TARGET): $(OBJECTS)
# 	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

# # Include generated dependency files
# -include $(DEPS)

# # Compilation rule for object files with automatic dependency generation
# $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
# 	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

# $(OBJ_DIR)/main.o: main.cpp | $(OBJ_DIR)
# 	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

# $(OBJ_DIR):
# 	mkdir -p $(OBJ_DIR)

# clean:
# 	rm -rf $(OBJ_DIR) $(TARGET) $(TARGET_1) $(TARGET_2) $(TARGET_3)

# # 單獨測試指令
# test_point_finding: $(OBJ_DIR)/test_point_finding.o $(OBJ_DIR)/tile.o $(OBJ_DIR)/corner_stitching.o
# 	$(CXX) $(CXXFLAGS) $^ -o $(TARGET_1)
# 	./$(TARGET_1)

# test_neighbor_finding: $(OBJ_DIR)/test_neighbor_finding.o $(OBJ_DIR)/tile.o $(OBJ_DIR)/corner_stitching.o
# 	$(CXX) $(CXXFLAGS) $^ -o $(TARGET_2)
# 	./$(TARGET_2)

# test_edge_tile_finding: $(OBJ_DIR)/test_edge_tile_finding.o $(OBJ_DIR)/tile.o $(OBJ_DIR)/corner_stitching.o
# 	$(CXX) $(CXXFLAGS) $^ -o $(TARGET_3)
# 	./$(TARGET_3) case_test.txt output1.txt test.txt

# # 編譯測試檔案
# $(OBJ_DIR)/test_point_finding.o: test_point_finding.cpp | $(OBJ_DIR)
# 	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

# $(OBJ_DIR)/test_neighbor_finding.o: test_neighbor_finding.cpp | $(OBJ_DIR)
# 	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

# $(OBJ_DIR)/test_edge_tile_finding.o: test_edge_tile_finding.cpp | $(OBJ_DIR)
# 	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

# # 編譯其他必要的物件檔案
# $(OBJ_DIR)/tile.o: $(SRC_DIR)/Tile.cpp | $(OBJ_DIR)
# 	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

# $(OBJ_DIR)/corner_stitching.o: $(SRC_DIR)/Corner_Stitching.cpp | $(OBJ_DIR)
# 	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

main: main.cpp 
	g++ -o Lab1 main.cpp -std=c++17
	./Lab1 case_0.txt output0.txt test.txt
	python3 draw_block_layout.py test.txt test.png

