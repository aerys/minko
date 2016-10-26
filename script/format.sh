DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

pushd ${DIR}/.. > /dev/null

echo "==== Formatting code for framework ===="
find framework -path framework/lib -prune -o \( -name '*.cpp' -or -name '*.hpp' \) -print0 | xargs -0 clang-format -style=file -i

echo "==== Formatting code for plugin ===="
find plugin \( -path "plugin/*/lib" \) -prune -o \( -name '*.cpp' -or -name '*.hpp' \) -print0 | xargs -0 clang-format -style=file -i

echo "==== Formatting code for test ===="
find test \( -name '*.cpp' -or -name '*.hpp' \) -print0 | xargs -0 clang-format -style=file -i

echo "==== Formatting code for example ===="
find example \( -name '*.cpp' -or -name '*.hpp' \) -print0 | xargs -0 clang-format -style=file -i

echo "==== Formatting code for tutorial ===="
find tutorial \( -name '*.cpp' -or -name '*.hpp' \) -print0 | xargs -0 clang-format -style=file -i

popd > /dev/null