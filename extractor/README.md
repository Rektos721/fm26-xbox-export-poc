# Extractor

This folder is the first step toward a stable FM26 data pipeline.

## Goal

Produce JSON bundles that match `interfaces.ts` and can later be inserted into `schema_v1.sql`.

## Immediate scope

- `players`
- `staff`

## Current state

- `bundle.ts`: bundle contract for extracted data
- `write-sample.ts`: tiny helper to emit a sample JSON file
- `samples/export.sample.json`: example output shape

## Planned runtime path

1. Use the native payload to locate the live squad/staff UI branches.
2. Read row/header data from the UI tree.
3. Normalize the result into `Player` / `Staff` objects.
4. Save a JSON export bundle.
