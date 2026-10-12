import { useEffect, useState } from 'react'
import { Link, useNavigate } from 'react-router-dom'
import { api } from '../lib/api'
import { useAuth } from '../context/AuthContext'
import AppHeader from '../components/AppHeader'
import StrawberryLogo from '../components/StrawberryLogo'
import { IconWallet, IconShield, IconLogout } from '../components/Icons'

const rupiah = (n) => 'Rp ' + Number(n).toLocaleString('id-ID')

function Row({ to, onClick, icon, title, sub, danger }) {
  const inner = (
    <>
      <span className="qa-ico" style={{ width:42, height:42, fontSize:'1rem', background: danger ? 'var(--berry-100)' : 'var(--matcha-100)', boxShadow:'3px 3px 0 var(--ink)' }}>{icon}</span>
      <div className="tx-main">
        <div className="tx-title" style={{ color: danger ? 'var(--berry-700)' : 'inherit' }}>{title}</div>
        {sub && <div className="tx-sub">{sub}</div>}
      </div>
      <span className="muted">›</span>
    </>
  )
  if (to) return <Link className="tx-row" to={to}>{inner}</Link>
  return <button className="tx-row" onClick={onClick} style={{ background:'transparent', border:'none', boxShadow:'none', padding:'0.6rem 0.2rem', width:'100%', color:'inherit', textAlign:'left', font:'inherit' }}>{inner}</button>
}

export default function Akun() {
  const [me, setMe] = useState(null)
  const { logout } = useAuth()
  const nav = useNavigate()

  useEffect(() => { api.me().then(setMe).catch(()=>{}) }, [])

  const doLogout = async () => { await logout(); nav('/') }

  return (
    <>
      <AppHeader title="Akun" back={false} />
      <div style={{ padding:'0 0.9rem', display:'grid', gap:'1rem' }}>
        <section className="card" style={{ display:'flex', gap:'0.9rem', alignItems:'center' }}>
          <div className="avatar" style={{ width:60, height:60, background:'var(--berry-100)' }}>
            <StrawberryLogo size={44} />
          </div>
          <div style={{ minWidth:0 }}>
            <h2 style={{ fontSize:'1.3rem' }}>{me?.name || '…'}</h2>
            <div className="muted mono" style={{ fontSize:'0.85rem' }}>ID {me?.id || '—'} · {me?.accounts?.length || 0} rekening</div>
            <div className="muted" style={{ fontSize:'0.8rem' }}>Total {me ? rupiah(me.totalBalance) : '—'}</div>
          </div>
        </section>

        <section className="card pad-sm">
          <div className="divider-list">
            <Row to="/accounts" icon={<IconWallet size={20} />} title="Rekening Saya" sub="Lihat rekening & mutasi" />
            <Row to="/akun/pin" icon={<IconShield size={20} />} title="Ganti PIN" sub="Perbarui PIN keamanan" />
            <Row onClick={doLogout} icon={<IconLogout size={20} />} title="Keluar" sub="Keluar dari akun ini" danger />
          </div>
        </section>

        <p className="muted" style={{ textAlign:'center', fontSize:'0.78rem' }}>
          Strawcha Bank (STC) · data disimpan in-memory
        </p>
      </div>
    </>
  )
}
