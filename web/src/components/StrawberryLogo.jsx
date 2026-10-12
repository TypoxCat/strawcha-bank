// Strawberry logo for Strawcha Bank (STC)
export function StrawberryLogo({ size = 40, className = '', style }) {
  return (
    <svg
      width={size} height={size} viewBox="0 0 64 64"
      className={className} style={style} role="img" aria-label="Strawcha Bank"
    >
      <defs>
        <linearGradient id="stc-body" x1="0" y1="0" x2="0" y2="1">
          <stop offset="0%" stopColor="#F4576F" />
          <stop offset="100%" stopColor="#C93E5C" />
        </linearGradient>
      </defs>

      <path
        d="M32 58C20 58 10 47 10 35C10 22 20 13 32 13C44 13 54 22 54 35C54 47 44 58 32 58Z"
        fill="url(#stc-body)" stroke="#3E5730" strokeWidth="2.4" strokeLinejoin="round"
      />

      <path d="M24 22C19 26 17 33 18 39" fill="none" stroke="#FFC2CE" strokeWidth="3" strokeLinecap="round" opacity="0.85" />

      <circle cx="26" cy="30" r="1.9" fill="#FFE1A8" />
      <circle cx="38" cy="28" r="1.9" fill="#FFE1A8" />
      <circle cx="32" cy="37" r="1.9" fill="#FFE1A8" />
      <circle cx="22" cy="40" r="1.9" fill="#FFE1A8" />
      <circle cx="42" cy="40" r="1.9" fill="#FFE1A8" />
      <circle cx="32" cy="47" r="1.9" fill="#FFE1A8" />

      <path d="M16 21C22 15 27 15 32 20C37 15 42 15 48 21C42 26 37 27 32 25C27 27 22 26 16 21Z"
            fill="#5F7A4E" stroke="#3E5730" strokeWidth="2.2" strokeLinejoin="round" />
      <rect x="30.6" y="3" width="2.8" height="7" rx="1.4" fill="#3E5730" />
    </svg>
  )
}

export default StrawberryLogo
