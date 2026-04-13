import type { Player, Staff } from "../interfaces";

export interface ExportBundle {
  players: Player[];
  staff: Staff[];
  exportedAt: string;
  source: "fm26-xbox-runtime" | "manual" | "unknown";
}

export function createEmptyBundle(): ExportBundle {
  return {
    players: [],
    staff: [],
    exportedAt: new Date().toISOString(),
    source: "unknown",
  };
}
