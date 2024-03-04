#pragma once

#include <memory>     // For std::unique_ptr
#include <stdexcept>  // For exceptions
#include <vector>

namespace USTC_CG
{
class Image
{
   public:
    Image() = default;

    // Constructor with width, height, and channels
    Image(int width, int height, int channels)
        : width_(width),
          height_(height),
          channels_(channels),
          image_data_(
              std::make_unique<unsigned char[]>(width * height * channels))
    {
    }

    // Constructor with width, height, channels, and external data
    Image(
        int width,
        int height,
        int channels,
        std::unique_ptr<unsigned char[]> image_data)
        : width_(width),
          height_(height),
          channels_(channels),
          image_data_(std::move(image_data))
    {
    }

    // Method to initialize or reinitialize from external data
    void initialize(
        int width,
        int height,
        int channels,
        std::unique_ptr<unsigned char[]> image_data)
    {
        width_ = width;
        height_ = height;
        channels_ = channels;
        image_data_ = std::move(image_data);
    }

    Image(const Image& other)
        : width_(other.width_),
          height_(other.height_),
          channels_(other.channels_),
          image_data_(std::make_unique<unsigned char[]>(
              other.width_ * other.height_ * other.channels_))
    {
        std::copy(
            other.image_data_.get(),
            other.image_data_.get() + width_ * height_ * channels_,
            image_data_.get());
    }

    Image& operator=(const Image& other)
    {
        if (this != &other)
        {
            width_ = other.width_;
            height_ = other.height_;
            channels_ = other.channels_;
            image_data_ =
                std::make_unique<unsigned char[]>(width_ * height_ * channels_);
            std::copy(
                other.image_data_.get(),
                other.image_data_.get() + width_ * height_ * channels_,
                image_data_.get());
        }
        return *this;
    }

    Image(Image&&) noexcept = default;
    Image& operator=(Image&&) noexcept = default;

    virtual ~Image() = default;

    // Getters for width, height, and channels
    int width() const
    {
        return width_;
    }
    int height() const
    {
        return height_;
    }
    int channels() const
    {
        return channels_;
    }

    unsigned char* data() const
    {
        return image_data_.get();
    }

    std::vector<unsigned char> get_pixel(int x, int y) const
    {
        if (x < 0 || x >= width_ || y < 0 || y >= height_)
        {
            throw std::out_of_range("Pixel coordinates out of bounds");
        }
        std::vector<unsigned char> pixelValues(channels_);
        std::size_t index =
            static_cast<std::size_t>(y) * static_cast<std::size_t>(width_) +
            static_cast<std::size_t>(x);
        index *= static_cast<std::size_t>(channels_);
        for (int channel = 0; channel < channels_; ++channel)
        {
            pixelValues[channel] = image_data_[index + channel];
        }
        return pixelValues;
    }

    void set_pixel(int x, int y, const std::vector<unsigned char>& values)
    {
        if (x < 0 || x >= width_ || y < 0 || y >= height_)
        {
            throw std::out_of_range("Pixel coordinates out of bounds");
        }
        // Allow 3 channel input when channels.size()==4 (RGB -> RGBA)
        size_t channels_reset = channels_;
        if (values.size() == 3 && static_cast<size_t>(channels_) == 4)
            channels_reset = 3;
        else if (values.size() != static_cast<size_t>(channels_))
        {
            throw std::invalid_argument(
                "Number of values does not match the number of channels");
        }
        std::size_t index =
            static_cast<std::size_t>(y) * static_cast<std::size_t>(width_) +
            static_cast<std::size_t>(x);
        index *= static_cast<std::size_t>(channels_);
        for (int channel = 0; channel < channels_reset; ++channel)
        {
            image_data_[index + channel] = values[channel];
        }
    }

   private:
    int width_ = 0, height_ = 0, channels_ = 0;
    std::unique_ptr<unsigned char[]> image_data_;
};
}  // namespace USTC_CG
