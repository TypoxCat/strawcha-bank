import { Link, useLocation } from 'react-router-dom'

const common = { width: 24, height: 24, viewBox: '0 0 24 24', fill: 'none', stroke: 'currentColor', strokeWidth: 2, strokeLinecap: 'round', strokeLinejoin: 'round' }

export function NavIcon({ name }) {
  switch (name) {
    case 'home': return <svg {...common}><path d="M3 11l9-8 9 8" /><path d="M5 10v10h5v-6h4v6h5V10" /></svg>
    case 'arrows': return <svg {...common}><path d="M4 8h13l-3-3M20 16H7l3 3" /></svg>
    case 'pay': return <svg {...common}><path d="M4 7h16v11H4z" /><path d="M4 11h16" /></svg>
    case 'clock': return <svg {...common}><circle cx="12" cy="12" r="9" /><path d="M12 7v5l3 2" /></svg>
    case 'user': return <svg {...common}><circle cx="12" cy="8" r="4" /><path d="M4 20c1.5-4 4.5-6 8-6s6.5 2 8 6" /></svg>
    default: return null
  }
}

export const TABS = [
  ['/dashboard', 'Beranda', 'home'],
  ['/transfer', 'Transfer', 'arrows'],
  ['/payments', 'Bayar', 'pay'],
  ['/history', 'Riwayat', 'clock'],
  ['/akun', 'Akun', 'user'],
]

export default function BottomNav() {
  const { pathname } = useLocation()
  const isActive = (to) => pathname === to || (to !== '/dashboard' && pathname.startsWith(to))
  return (
    <nav className="bottomnav" aria-label="Navigasi utama">
      <div className="bottomnav-inner">
        {TABS.map(([to, label, icon]) => (
          <Link key={to} to={to} className={isActive(to) ? 'bn-item active' : 'bn-item'}>
            <span className="bn-ico"><NavIcon name={icon} /></span>
            <span className="bn-label">{label}</span>
          </Link>
        ))}
      </div>
    </nav>
  )
}
