import { writeFileSync } from "node:fs";
import { resolve } from "node:path";
import { createEmptyBundle } from "./bundle";

const bundle = createEmptyBundle();
bundle.source = "manual";

const out = resolve(process.cwd(), "extractor", "samples", "export.sample.json");
writeFileSync(out, JSON.stringify(bundle, null, 2), "utf8");
console.log(`Wrote ${out}`);
