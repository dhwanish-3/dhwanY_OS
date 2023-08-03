#!/bin/sh

download() {
    component="$1"
    echo "Downloading $component .. "
    wget "https://github.com/eXpOSNitc/$component/archive/master.zip" -c -q -O "$component.zip"
    unzip -q "$component".zip
    mv "$component"-master "$component"
    rm "$component".zip
    echo "$component downloaded."
}

myexpos() {
    download expl
    download spl
    download xfs-interface
    download xsm
}

bootstrap() {
    if [ -e myexpos ]; then
        echo "myexpos folder exists."
        echo "Please remove it and re-run the script to download again."
        return
    fi;

    wget -q https://github.com/eXpOSNitc/expos-bootstrap/archive/main.zip
    unzip -q main.zip
    rm main.zip
    mv expos-bootstrap-main myexpos
    cd myexpos
    ./download.sh
    rm Readme.md # Because the Readme is for the repo and will confuse if placed
                 # in myexpos folder
    echo "Download complete."
    cd ..
}


if [ -f "Makefile" ]; then
    # If Makefile is there, continue downloading myexpos components
    myexpos
else
    # The user is executing directly from `wget | sh`
    # so create the folder and download makefile
    bootstrap

    # PS: We could have just downloaded the Makefile alone in the bootstrap
    # function. But downloading the repo provides us the possibilty to provide
    # additional helper files in the future.
fi
