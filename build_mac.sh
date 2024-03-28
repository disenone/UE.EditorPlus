#!/bin/bash
set -x

CWD=$(cd $(dirname $0);pwd)
ENGINE=/Volumes/SSD/UE/Engines/UE_
OUTPUT=/Volumes/SSD/UE/projects/products
VERS=(5.1 5.2 5.3)
ZIP=zip

help(){
    echo "Usage: bash build_mac.sh [all|5.0|5.1|5.2|5.3]"
}

package(){
    VER=$1
    POUTPUT=${OUTPUT}/EditorPlus/ue${VER}/EditorPlus
    echo "--------------------Building Version [${VER}]--------------------"
    "${ENGINE}${VER}/Engine/Build/BatchFiles/RunUAT.sh" BuildPlugin -Plugin="${CWD}/EditorPlus.uplugin" -Package="${POUTPUT}" -Rocket
    mkdir "${POUTPUT}/Config"
    cp "${CWD}/Config/FilterPlugin.ini" "${POUTPUT}/Config/"
    rm -rf "${POUTPUT}/Binaries"
    rm -rf "${POUTPUT}/Intermediate"
    cd "${POUTPUT}/../"
    rm -rf "*.zip"
    ${ZIP} -r -q "EditorPlus_ue${VER}.zip" "EditorPlus"
    cp *.zip "${OUTPUT}/EditorPlus/"
    cd "${CWD}"
}


ARG=$1

if [ -z "${ARG}" ]; then
    help
    exit
fi

# check version valid
if [[ "${VERS[@]}"  =~ "${ARG}" ]]; then
    package ${ARG}

elif [ "${ARG}" = "all" ]; then
    for ver in ${VERS[@]}
    do
	    package ${ver}
    done

else
    echo "Engine Version [${ARG}] invalid"
    help
fi
