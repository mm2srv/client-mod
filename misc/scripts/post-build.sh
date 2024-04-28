#!/bin/bash
set -e

OUT_NSO=${OUT}/${BINARY_NAME}
OUT_NPDM=${OUT}/main.npdm

# Clear older build.
rm -rf ${OUT}

# Create out directory.
mkdir -p ${OUT}

# Copy build into out
mv ${NAME}.nso ${OUT_NSO}
mv ${NAME}.npdm ${OUT_NPDM}

# Copy ELF to user path if defined.
if [ ! -z $ELF_EXTRACT ]; then
    cp "$NAME.elf" "$ELF_EXTRACT"
fi

(cd "$DEPLOY" && zip -r ocw-mod-atmosphere.zip atmosphere)
(cd "$DEPLOY" && cp -r atmosphere/contents/01009B90006DC000 opencourseworld-mod && zip -r ocw-mod-ryujinx.zip opencourseworld-mod)
