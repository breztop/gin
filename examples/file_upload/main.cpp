#include <iostream>
#include <fstream>
#include <filesystem>

#include "core/app.hpp"

int main() {
    auto app = gin::App::Default();

    app.Get("/", [](gin::Context& ctx) {
        std::ifstream file("examples/file_upload/index.html");
        if (!file) {
            ctx.String(404, "HTML file not found");
            return;
        }
        std::stringstream ss;
        ss << file.rdbuf();
        ctx.HTML(200, ss.str());
    });

    app.Post("/upload", [](gin::Context& ctx) {
        auto* file = ctx.GetFile("file");
        if (!file) {
            ctx.JSON(400, {{"error", "No file uploaded"}});
            return;
        }

        auto filename = file->GetFilename();
        if (filename.empty()) {
            filename = "uploaded_file";
        }

        std::filesystem::create_directories("uploads");
        auto filepath = "uploads/" + filename;

        if (file->Save(filepath)) {
            ctx.JSON(200, {
                {"message", "File uploaded successfully"},
                {"filename", filename},
                {"size", file->GetSize()}
            });
        } else {
            ctx.JSON(500, {{"error", "Failed to save file"}});
        }
    });

    std::cout << "Starting server on http://127.0.0.1:8080" << std::endl;
    app.Run(8080);

    return 0;
}
