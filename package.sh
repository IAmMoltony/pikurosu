#!/usr/bin/bash
#Script for packaging Pikurosu

pkgVersion="$1"
fileName="Pikurosu-$pkgVersion.zip"
mkdir -p "pkg-zip"
cp -r fonts pkg-zip/
cp -r levels pkg-zip/
cp Pikurosu pkg-zip/
mv pkg-zip "pikurosu$pkgVersion"
zip -r $fileName "pikurosu$pkgVersion"
rm -r "pikurosu$pkgVersion"
