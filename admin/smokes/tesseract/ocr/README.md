## Tesseract deterministic OCR smoke

Tesseract 5.5.3 has a second `ocr` FSM smoke, preserving the original model-free `consumer` smoke. The pinned official `tessdata_fast` language model comes from commit `87416418657359cb625c412a48b6e1d6d41c29bd`, SHA-256 `7d4322bd2a7749724879683fc3912cb542f19906c83bcc1a52132556427170b2`. The fixture is synthetic PNG with SHA-256 `11562c70532c2ab800dc5a2464daac40763e2db948aae6e122307f2838e712c2`. Expected OCR: `OCR TEST 4711`.

The model is provisioned via XML, installed at `share/tessdata/eng.traineddata`, and available at configure time. The test executes without CLI arguments. Linux source-level recognition passed; BCC64X execution is not yet verified.
