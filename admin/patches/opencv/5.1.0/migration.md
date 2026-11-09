# OpenCV 5.1.0 BCC64X migration

- Pin OpenCV to tag 5.1.0 commit e4f78fbcf1a773fbed69e14500e8b734963eb585.
- Pin opencv_contrib to matching tag commit b67a855188a3d683813731010f6d39ac614733a4.
- Rebase the optional DNN/MLAS patch to 5.1.0. Keep OPENCV_DNN_MLAS=OFF until native MLAS is verified.
- Change installed OpenCV module DLL/import library suffix from 500 to 510.
- Keep existing TBB, OpenMP, image codecs, modules and consumer profile unchanged.
- Run Release and Debug, tests, install, package smoke, metadata, license/SBOM, publish and Doxygen on Windows BCC64X before claiming PASS.
- Inspect unexpected build duration with logs before reducing or replacing bundled OpenCV codecs.
