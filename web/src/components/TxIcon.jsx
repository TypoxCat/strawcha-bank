import { IconTransfer, IconTopUp, IconBill, IconWallet } from './Icons'

export function txMeta(t) {
  const name = t?.typeName
  if (name === 'Transfer') return { label: 'Transfer', Icon: IconTransfer, bg: 'var(--berry-200)', fg: 'var(--berry-700)' }
  if (name === 'E-Wallet Top Up') return { label: 'Top Up E-Wallet', Icon: IconTopUp, bg: 'var(--matcha-200)', fg: 'var(--matcha-700)' }
  if (name === 'Bill Payment') return { label: 'Pembayaran Tagihan', Icon: IconBill, bg: 'var(--butter)', fg: '#8a6b1d' }
  return { label: name || 'Transaksi', Icon: IconWallet, bg: 'var(--matcha-100)', fg: 'var(--matcha-700)' }
}

export default function TxIcon(t) {
  if (t && t.typeName === undefined && t.t && typeof t.t === 'object') t = t.t
  const m = txMeta(t)
  return (
    <span
      aria-hidden
      style={{
        width: 44, height: 44, flex: '0 0 44px',
        display: 'grid', placeItems: 'center',
        background: m.bg, color: m.fg,
        border: '2px solid var(--ink)', borderRadius: 999,
      }}
    >
      <m.Icon size={20} />
    </span>
  )
}
