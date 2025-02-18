#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <H5Cpp.h>

using namespace H5;

struct HDF5Node {
    std::string name;
    std::map<std::string, std::string> attributes;
    std::map<std::string, std::vector<float>> float_datasets;
    std::map<std::string, std::vector<int>> int_datasets;
    std::map<std::string, std::vector<unsigned char>> byte_datasets;
    std::map<std::string, std::vector<double>> double_datasets;
    std::map<std::string, std::string> string_datasets;
    std::vector<HDF5Node> subgroups;
};

void readAttributes(H5Object &obj, HDF5Node &node) {
    for (hsize_t i = 0; i < obj.getNumAttrs(); ++i) {
        Attribute attr = obj.openAttribute(i);
        H5std_string attr_name = attr.getName();
        DataType type = attr.getDataType();
        if (type == PredType::NATIVE_INT) {
            int value;
            attr.read(type, &value);
            node.attributes[attr_name] = std::to_string(value);
        } else if (type == PredType::NATIVE_FLOAT) {
            float value;
            attr.read(type, &value);
            node.attributes[attr_name] = std::to_string(value);
        } else if (type == StrType(0, H5T_VARIABLE)) {
            H5std_string value;
            attr.read(type, value);
            node.attributes[attr_name] = value;
        }
    }
}

void readGroup(Group &group, HDF5Node &node) {
    readAttributes(group, node);

    for (hsize_t i = 0; i < group.getNumObjs(); ++i) {
        H5std_string name = group.getObjnameByIdx(i);
        H5G_obj_t type = group.getObjTypeByIdx(i);

        if (type == H5G_GROUP) {
            Group subgroup = group.openGroup(name);
            HDF5Node subnode;
            subnode.name = name;
            readGroup(subgroup, subnode);
            node.subgroups.push_back(subnode);
        } else if (type == H5G_DATASET) {
            DataSet dataset = group.openDataSet(name);
            DataType dtype = dataset.getDataType();
            DataSpace dspace = dataset.getSpace();
            hsize_t size = dspace.getSimpleExtentNpoints();

            if (dtype == PredType::NATIVE_INT) {
                std::vector<int> data(size);
                dataset.read(data.data(), dtype);
                node.int_datasets[name] = data;
            } else if (dtype == PredType::NATIVE_FLOAT) {
                std::vector<float> data(size);
                dataset.read(data.data(), dtype);
                node.float_datasets[name] = data;
            } else if (dtype == PredType::NATIVE_DOUBLE) {
                std::vector<double> data(size);
                dataset.read(data.data(), dtype);
                node.float_datasets[name] = std::vector<float>(data.begin(), data.end());
            } else if (dtype == PredType::NATIVE_CHAR) {
                std::vector<char> data(size);
                dataset.read(data.data(), dtype);
                node.string_datasets[name] = std::string(data.begin(), data.end());
            } else if (dtype == StrType(0, H5T_VARIABLE)) {
                H5std_string data;
                dataset.read(data, dtype);
                node.string_datasets[name] = data;
            } else if (dtype == PredType::NATIVE_UCHAR) {
                std::vector<unsigned char> data(size);
                dataset.read(data.data(), dtype);
                node.byte_datasets[name] = data;
            } else {
                std::cout << "不存在这种类型的数据" << std::endl;
            }
        }
    }
}

void printNode(const HDF5Node &node, int indent = 0) {
    std::string indent_str(indent, ' ');

    std::cout << indent_str << "Group: " << node.name << std::endl;

    std::cout << indent_str << "Attributes:" << std::endl;
    for (const auto &attr : node.attributes) {
        std::cout << indent_str << "  " << attr.first << ": " << attr.second << std::endl;
    }

    std::cout << indent_str << "Integer Datasets:" << std::endl;
    for (const auto &dataset : node.int_datasets) {
        std::cout << indent_str << "  " << dataset.first << ": ";
        for (int value : dataset.second) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }

    std::cout << indent_str << "Float Datasets:" << std::endl;
    for (const auto &dataset : node.float_datasets) {
        std::cout << indent_str << "  " << dataset.first << ": ";
        for (float value : dataset.second) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }

    std::cout << indent_str << "Double Datasets:" << std::endl;
    for (const auto &dataset : node.double_datasets) {
        std::cout << indent_str << "  " << dataset.first << ": ";
        for (double value : dataset.second) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }

    std::cout << indent_str << "Byte Datasets:" << std::endl;
    for (const auto &dataset : node.byte_datasets) {
        std::cout << indent_str << "  " << dataset.first << ": ";
        for (unsigned char value : dataset.second) {
            std::cout << static_cast<int>(value) << " ";
        }
        std::cout << std::endl;
    }

    std::cout << indent_str << "String Datasets:" << std::endl;
    for (const auto &dataset : node.string_datasets) {
        std::cout << indent_str << "  " << dataset.first << ": " << dataset.second << std::endl;
    }

    std::cout << indent_str << "Subgroups:" << std::endl;
    for (const auto &subgroup : node.subgroups) {
        printNode(subgroup, indent + 2);
    }
}

HDF5Node read_hdf5(const H5std_string &FILE_NAME) {
    HDF5Node root;
    root.name = "root";
    try {
        const H5File file(FILE_NAME, H5F_ACC_RDONLY);
        Group root_group = file.openGroup("/");
        readGroup(root_group, root);
    } catch (FileIException &error) {
        error.printErrorStack();
    } catch (DataSetIException &error) {
        error.printErrorStack();
    } catch (GroupIException &error) {
        error.printErrorStack();
    }
    return root;
}


int main() {
    const H5std_string FILE_NAME("/home/snivyer/SemanticDataService/data/FY3/CLM/FY3D_MERSI_GBAL_L2_CLM_MLT_GLL_20230106_POAD_5000M_MS.HDF");
    HDF5Node root = read_hdf5(FILE_NAME);
    printNode(root);  
    
    // 这里可以添加更多代码来遍历和输出root的内容
    return 0;
}