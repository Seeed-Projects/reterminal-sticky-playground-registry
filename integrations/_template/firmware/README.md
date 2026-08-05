# Firmware packages

Create one directory per version. After building the source project, run:

```bash
npm run package:esp-idf -- <integration-id> <version>
```

Commit the generated `manifest.json` and every `.bin` file required by the
manifest.
