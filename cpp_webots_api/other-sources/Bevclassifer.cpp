#include <torch/script.h> // One-stop header.
#include <opencv2/opencv.hpp>
#include <iostream>
#include <memory>

//https://pytorch.org/tutorials/advanced/cpp_export.html

string image_path = '/home/yingkai/transfer_tutorial/example';

void bevclassifier(Mat image_transfomed) {

    // Deserialize the ScriptModule from a file using torch::jit::load().
    std::shared_ptr<torch::jit::script::Module> module = torch::jit::load('/home/yingkai/transfer_tutorial/transfer.pt');

    assert(module != nullptr);
    std::cout << "ok\n";


    // //输入图像
    // auto image = cv::imread(image_path +"/"+ "1.png",cv::ImreadModes::IMREAD_COLOR);
    // cv::Mat image_transfomed;
    // cv::resize(image, image_transfomed, cv::Size(224, 224));
    // cv::cvtColor(image_transfomed, image_transfomed, cv::COLOR_BGR2RGB);

    // 图像转换为Tensor
    torch::Tensor tensor_image = torch::from_blob(image_transfomed.data, {image_transfomed.rows, image_transfomed.cols,3},torch::kByte);
    tensor_image = tensor_image.permute({2,0,1});
    tensor_image = tensor_image.toType(torch::kFloat);
    tensor_image = tensor_image.div(255);

    tensor_image = tensor_image.unsqueeze(0);


    // 网络前向计算
    // Execute the model and turn its output into a tensor.
    at::Tensor output = module->forward({tensor_image}).toTensor();

    auto max_result = output.max(1,true);
    auto max_index = std::get<1>(max_result).item<float>();

    if (max_index == 0){
        std::cout << "left";
    }else{
        std::cout << "right";;
    }

    //cv::imshow("image", image);
    //cv::waitKey(0);


//    at::Tensor prob = torch::softmax(output,1);
//    auto prediction = torch::argmax(output, 1);
//
//    auto aa = prediction.slice(/*dim=*/0, /*start=*/0, /*end=*/2).item();
//
//    std::cout << output.slice(/*dim=*/1, /*start=*/0, /*end=*/2) << '\n';
//    std::cout << prob.slice(/*dim=*/1, /*start=*/0, /*end=*/2) << '\n';
//    std::cout <<prediction.slice(/*dim=*/0, /*start=*/0, /*end=*/2)<<"\n";

}

