export type PositionCode =
  | "GK"
  | "DL"
  | "DR"
  | "DC"
  | "DM"
  | "ML"
  | "MR"
  | "MC"
  | "AML"
  | "AMR"
  | "AMC"
  | "ST";

export type Foot = "left" | "right" | "either" | "unknown";

export interface ContractInfo {
  wage?: number;
  value?: number;
  contractEnd?: string;
  releaseClause?: number;
}

export interface PhysicalAttributes {
  acceleration?: number;
  agility?: number;
  balance?: number;
  jumpingReach?: number;
  naturalFitness?: number;
  pace?: number;
  stamina?: number;
  strength?: number;
}

export interface MentalAttributes {
  aggression?: number;
  anticipation?: number;
  bravery?: number;
  composure?: number;
  concentration?: number;
  decisions?: number;
  determination?: number;
  flair?: number;
  leadership?: number;
  offTheBall?: number;
  positioning?: number;
  teamwork?: number;
  vision?: number;
  workRate?: number;
}

export interface TechnicalAttributes {
  corners?: number;
  crossing?: number;
  dribbling?: number;
  finishing?: number;
  firstTouch?: number;
  freeKickTaking?: number;
  heading?: number;
  longShots?: number;
  longThrows?: number;
  marking?: number;
  passing?: number;
  penaltyTaking?: number;
  tackling?: number;
  technique?: number;
}

export interface GoalkeepingAttributes {
  aerialReach?: number;
  commandOfArea?: number;
  communication?: number;
  eccentricity?: number;
  handling?: number;
  kicking?: number;
  oneOnOnes?: number;
  reflexes?: number;
  rushingOut?: number;
  punching?: number;
  throwing?: number;
}

export interface PlayerAttributes {
  physical: PhysicalAttributes;
  mental: MentalAttributes;
  technical: TechnicalAttributes;
  goalkeeping?: GoalkeepingAttributes;
}

export interface RoleFitScore {
  role: string;
  score: number;
  duty?: string;
}

export interface Player {
  id: string;
  name: string;
  age: number;
  club?: string;
  nation?: string;
  positions: PositionCode[];
  preferredFoot: Foot;
  heightCm?: number;
  weightKg?: number;
  personality?: string;
  mediaHandling?: string;
  attributes: PlayerAttributes;
  condition?: number;
  matchSharpness?: number;
  morale?: string;
  form?: string;
  injuryStatus?: string;
  currentSignal?: number;
  potentialSignal?: number;
  roleFitScores?: RoleFitScore[];
  contract?: ContractInfo;
}

export interface YouthPlayer extends Player {
  youthIntakeYear?: number;
  prospectGrade?: string;
  potentialRangeText?: string;
  developmentNotes?: string;
  scoutRecommendation?: string;
  homegrownStatus?: string;
}

export interface StaffAttributes {
  attacking?: number;
  defending?: number;
  fitness?: number;
  goalkeeping?: number;
  mental?: number;
  tactical?: number;
  technical?: number;
  judgingPlayerAbility?: number;
  judgingPlayerPotential?: number;
  adaptability?: number;
  determination?: number;
  levelOfDiscipline?: number;
  motivating?: number;
  peopleManagement?: number;
  workingWithYoungsters?: number;
  tacticalKnowledge?: number;
}

export interface Staff {
  id: string;
  name: string;
  role: string;
  club?: string;
  nation?: string;
  attributes: StaffAttributes;
  contract?: ContractInfo;
}

export interface RoleDepthEntry {
  role: string;
  players: string[];
  strengthScore?: number;
}

export interface TeamContext {
  teamName: string;
  formation?: string;
  registeredPlayers?: string[];
  unregisteredPlayers?: string[];
  injuredPlayers?: string[];
  tacticalFamiliarity?: Record<string, number>;
  roleDepth?: RoleDepthEntry[];
}

export interface TacticalRoleAssignment {
  slot: string;
  role: string;
  duty?: string;
  playerId?: string;
}

export interface Tactic {
  formation: string;
  mentality?: string;
  inPossession?: string[];
  inTransition?: string[];
  outOfPossession?: string[];
  playerRoles: TacticalRoleAssignment[];
  setPieceNotes?: string[];
}
