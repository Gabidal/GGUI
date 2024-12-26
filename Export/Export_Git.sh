# Function to check if the script is run from the project root directory or a subdirectory.
check_directory() {
    local current_dir=$(pwd)
    local project_root_name="GGUI"
    local bin_dir_name="Export"

    # Find the project root directory by looking for the .git directory
    project_root=$(git rev-parse --show-toplevel 2>/dev/null)

    # If git is not found or the project root is not determined
    if [ -z "$project_root" ]; then
        echo "Error: Unable to determine the project root directory. Ensure you're in the GGUI project."
        exit 1
    fi

    # If we're in the project root, change to the 'bin' directory
    if [ "$(basename "$project_root")" == "$project_root_name" ] && [ "$current_dir" == "$project_root" ]; then
        echo "Project root directory detected. Changing to the 'bin' directory."
        cd "$project_root/$bin_dir_name" || exit 1
    # Otherwise, navigate to the 'bin' directory from anywhere in the project
    elif [[ "$current_dir" != *"$project_root/$bin_dir_name"* ]]; then
        echo "Navigating to the 'bin' directory within the project."
        cd "$project_root/$bin_dir_name" || exit 1
    fi
}

check_directory

# Make sure that libgit2-dev is installed
if [ ! -f /usr/include/git2.h ]; then
    echo "libgit2-dev is not installed. Please install it using the following command:"
    echo "sudo apt-get install libgit2-dev"
    exit 1
fi

# We need to download some dependencies for this to work:
# URL="https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp"

# # Use curl to download the file and save it in the current directory
# curl -L -o json.hpp "$URL"

# # Check if the download was successful
# if [ $? -eq 0 ]; then
#     echo "json.hpp has been successfully downloaded."
# else
#     echo "Failed to download json.hpp."
# fi

g++ -g -o Export_Git ./Export_Git.cpp -I/usr/include -lgit2 -std=c++11

cd ..

gdb --args ./Export/Export_Git ./ Dev
# ./Export/Export_Git ./ Dev
