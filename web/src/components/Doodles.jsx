export function Strawberry({ style, size = 64 }) {
  return (
    <svg width={size} height={size} viewBox="0 0 64 64" className="doodle" style={style} aria-hidden>
      <ellipse cx="32" cy="40" rx="19" ry="13" fill="var(--berry-500)" stroke="var(--ink)" strokeWidth="2.2"/>
      <ellipse cx="23" cy="37" rx="6" ry="3.2" fill="var(--berry-200)"/>
      <circle cx="25" cy="34" r="1.8" fill="var(--ink)"/>
      <circle cx="37" cy="35" r="1.8" fill="var(--ink)"/>
      <circle cx="31" cy="42" r="1.8" fill="var(--ink)"/>
      <path d="M20 26 C17 17, 24 11, 27 15 C29 10, 36 11, 36 17 C40 13, 46 17, 42 24" fill="none" stroke="var(--matcha-700)" strokeWidth="2.4" strokeLinecap="round"/>
      <circle cx="27" cy="16" r="2.6" fill="var(--matcha-500)"/>
      <circle cx="35" cy="17" r="2.6" fill="var(--matcha-500)"/>
    </svg>
  )
}

export function MatchaLeaf({ style, size = 72 }) {
  return (
    <svg width={size} height={size} viewBox="0 0 72 72" className="doodle" style={style} aria-hidden>
      <path d="M20 52 C13 30, 27 14, 50 20 C34 27, 25 39, 20 52 Z" fill="var(--matcha-500)" stroke="var(--ink)" strokeWidth="2.2"/>
      <path d="M29 36 L42 25" stroke="var(--matcha-900)" strokeWidth="2" strokeLinecap="round"/>
      <path d="M28 43 L37 38" stroke="var(--matcha-900)" strokeWidth="2" strokeLinecap="round"/>
    </svg>
  )
}

export function BubbleTea({ style, size = 64 }) {
  return (
    <svg width={size} height={size} viewBox="0 0 64 64" className="doodle" style={style} aria-hidden>
      <path d="M22 20 L26 52 Q27 56 32 56 Q37 56 38 52 L42 20 Z" fill="#fff" stroke="var(--ink)" strokeWidth="2.2"/>
      <path d="M24 30 L40 30 L38 52 Q37 56 32 56 Q27 56 26 52 Z" fill="var(--matcha-300)"/>
      <path d="M20 16 L44 16 L42 22 L22 22 Z" fill="var(--berry-300)" stroke="var(--ink)" strokeWidth="2.2"/>
      <circle cx="28" cy="49" r="2.2" fill="var(--ink)"/>
      <circle cx="35" cy="51" r="2.2" fill="var(--ink)"/>
      <circle cx="31" cy="53" r="2.2" fill="var(--ink)"/>
    </svg>
  )
}
