#!/bin/bash

if [ $# -eq 0 ]; then
    project=minsky
else
    project=$1
fi
    
rm *.log
for i in Dockerfile-*[^~]; do
    case $i in
        Dockerfile-ubuntu) versions="18.04 20.04 22.04";;
        Dockerfile-fedora) versions="31 32 33 34 35 36";;
        *) versions=default;;
    esac
    for version in $versions; do
        if docker build --network=host --build-arg project=$project --build-arg version=$version --pull -f $i .; then
            echo "$i-$version PASSED" >$i-$version.log
        else
            echo "$i-$version FAILED" >$i-$version.log
        fi &
    done
done
wait
docker container prune -f
cat *.log
# check if any child process failed, and emit an appropriate status code
grep FAILED *.log &>/dev/null
exit $[!$?]
