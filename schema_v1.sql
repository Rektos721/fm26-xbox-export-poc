PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS players (
  id TEXT PRIMARY KEY,
  name TEXT NOT NULL,
  age INTEGER NOT NULL,
  club TEXT,
  nation TEXT,
  preferred_foot TEXT CHECK (preferred_foot IN ('left','right','either','unknown')) DEFAULT 'unknown',
  height_cm INTEGER,
  weight_kg INTEGER,
  personality TEXT,
  media_handling TEXT,
  condition_pct REAL,
  match_sharpness_pct REAL,
  morale TEXT,
  form TEXT,
  injury_status TEXT,
  current_signal REAL,
  potential_signal REAL,
  wage INTEGER,
  value INTEGER,
  contract_end TEXT,
  release_clause INTEGER,
  created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS player_positions (
  player_id TEXT NOT NULL,
  position_code TEXT NOT NULL,
  sort_order INTEGER NOT NULL DEFAULT 0,
  PRIMARY KEY (player_id, position_code),
  FOREIGN KEY (player_id) REFERENCES players(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS player_attributes (
  player_id TEXT PRIMARY KEY,
  acceleration INTEGER,
  agility INTEGER,
  balance INTEGER,
  jumping_reach INTEGER,
  natural_fitness INTEGER,
  pace INTEGER,
  stamina INTEGER,
  strength INTEGER,
  aggression INTEGER,
  anticipation INTEGER,
  bravery INTEGER,
  composure INTEGER,
  concentration INTEGER,
  decisions INTEGER,
  determination INTEGER,
  flair INTEGER,
  leadership INTEGER,
  off_the_ball INTEGER,
  positioning INTEGER,
  teamwork INTEGER,
  vision INTEGER,
  work_rate INTEGER,
  corners INTEGER,
  crossing INTEGER,
  dribbling INTEGER,
  finishing INTEGER,
  first_touch INTEGER,
  free_kick_taking INTEGER,
  heading INTEGER,
  long_shots INTEGER,
  long_throws INTEGER,
  marking INTEGER,
  passing INTEGER,
  penalty_taking INTEGER,
  tackling INTEGER,
  technique INTEGER,
  aerial_reach INTEGER,
  command_of_area INTEGER,
  communication INTEGER,
  eccentricity INTEGER,
  handling INTEGER,
  kicking INTEGER,
  one_on_ones INTEGER,
  reflexes INTEGER,
  rushing_out INTEGER,
  punching INTEGER,
  throwing INTEGER,
  FOREIGN KEY (player_id) REFERENCES players(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS youth_players (
  player_id TEXT PRIMARY KEY,
  youth_intake_year INTEGER,
  prospect_grade TEXT,
  potential_range_text TEXT,
  development_notes TEXT,
  scout_recommendation TEXT,
  homegrown_status TEXT,
  FOREIGN KEY (player_id) REFERENCES players(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS player_role_fit_scores (
  player_id TEXT NOT NULL,
  role_name TEXT NOT NULL,
  duty TEXT,
  score REAL NOT NULL,
  PRIMARY KEY (player_id, role_name, duty),
  FOREIGN KEY (player_id) REFERENCES players(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS staff (
  id TEXT PRIMARY KEY,
  name TEXT NOT NULL,
  role TEXT NOT NULL,
  club TEXT,
  nation TEXT,
  wage INTEGER,
  value INTEGER,
  contract_end TEXT,
  release_clause INTEGER,
  created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS staff_attributes (
  staff_id TEXT PRIMARY KEY,
  attacking INTEGER,
  defending INTEGER,
  fitness INTEGER,
  goalkeeping INTEGER,
  mental INTEGER,
  tactical INTEGER,
  technical INTEGER,
  judging_player_ability INTEGER,
  judging_player_potential INTEGER,
  adaptability INTEGER,
  determination INTEGER,
  level_of_discipline INTEGER,
  motivating INTEGER,
  people_management INTEGER,
  working_with_youngsters INTEGER,
  tactical_knowledge INTEGER,
  FOREIGN KEY (staff_id) REFERENCES staff(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS team_contexts (
  id TEXT PRIMARY KEY,
  team_name TEXT NOT NULL,
  formation TEXT,
  created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS team_context_players (
  team_context_id TEXT NOT NULL,
  player_id TEXT NOT NULL,
  bucket TEXT NOT NULL CHECK (bucket IN ('registered','unregistered','injured')),
  PRIMARY KEY (team_context_id, player_id, bucket),
  FOREIGN KEY (team_context_id) REFERENCES team_contexts(id) ON DELETE CASCADE,
  FOREIGN KEY (player_id) REFERENCES players(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS tactical_familiarity (
  team_context_id TEXT NOT NULL,
  scope_key TEXT NOT NULL,
  score REAL NOT NULL,
  PRIMARY KEY (team_context_id, scope_key),
  FOREIGN KEY (team_context_id) REFERENCES team_contexts(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS role_depth_entries (
  id TEXT PRIMARY KEY,
  team_context_id TEXT NOT NULL,
  role_name TEXT NOT NULL,
  strength_score REAL,
  FOREIGN KEY (team_context_id) REFERENCES team_contexts(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS role_depth_players (
  role_depth_entry_id TEXT NOT NULL,
  player_id TEXT NOT NULL,
  sort_order INTEGER NOT NULL DEFAULT 0,
  PRIMARY KEY (role_depth_entry_id, player_id),
  FOREIGN KEY (role_depth_entry_id) REFERENCES role_depth_entries(id) ON DELETE CASCADE,
  FOREIGN KEY (player_id) REFERENCES players(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS tactics (
  id TEXT PRIMARY KEY,
  formation TEXT NOT NULL,
  mentality TEXT,
  created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS tactic_instructions (
  tactic_id TEXT NOT NULL,
  phase TEXT NOT NULL CHECK (phase IN ('in_possession','in_transition','out_of_possession','set_piece_notes')),
  instruction TEXT NOT NULL,
  sort_order INTEGER NOT NULL DEFAULT 0,
  PRIMARY KEY (tactic_id, phase, sort_order),
  FOREIGN KEY (tactic_id) REFERENCES tactics(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS tactical_role_assignments (
  tactic_id TEXT NOT NULL,
  slot TEXT NOT NULL,
  role_name TEXT NOT NULL,
  duty TEXT,
  player_id TEXT,
  PRIMARY KEY (tactic_id, slot),
  FOREIGN KEY (tactic_id) REFERENCES tactics(id) ON DELETE CASCADE,
  FOREIGN KEY (player_id) REFERENCES players(id) ON DELETE SET NULL
);

CREATE INDEX IF NOT EXISTS idx_players_club ON players(club);
CREATE INDEX IF NOT EXISTS idx_players_age ON players(age);
CREATE INDEX IF NOT EXISTS idx_players_potential_signal ON players(potential_signal);
CREATE INDEX IF NOT EXISTS idx_staff_role ON staff(role);
CREATE INDEX IF NOT EXISTS idx_team_context_players_bucket ON team_context_players(bucket);
CREATE INDEX IF NOT EXISTS idx_role_depth_entries_team_context ON role_depth_entries(team_context_id);
CREATE INDEX IF NOT EXISTS idx_tactic_assignments_player ON tactical_role_assignments(player_id);
