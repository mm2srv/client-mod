# Build client mod
./exlaunch.sh build

# Create new mod layout
mkdir -p out/exefs
mkdir -p out/romfs
cp -r layeredfs/exefs/* out/exefs
cp deploy/* out/exefs

# Copy romfs
mkdir -p out/romfs/Message
cp layeredfs/romfs/Message/LayoutMsg_USen.szs.patch out/romfs/Message