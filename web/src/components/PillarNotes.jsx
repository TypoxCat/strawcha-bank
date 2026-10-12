import { useLocation } from 'react-router-dom'
import { PILLARS } from '../lib/oopNotes'
import { CodeBlock } from '../lib/highlight'

function Note({ n, side }) {
  return (
    <div className={'note ' + side}>
      <div className="note-bubble">
        <span className={'note-pillar ' + (n.tone || 'matcha')}>{n.pillar}</span>
        <p>{n.text}</p>
        {n.file && <div className="note-file">{n.file}</div>}
        {n.code && <CodeBlock code={n.code} />}
      </div>
    </div>
  )
}

export default function PillarNotes() {
  const { pathname } = useLocation()
  const key = pathname.startsWith('/accounts') ? '/accounts'
    : pathname.startsWith('/akun') ? '/akun'
    : pathname
  const cfg = PILLARS[key] || PILLARS['/dashboard']

  return (
    <>
      <aside className="pillar-notes left" aria-hidden>
        <div className="pillar-heading">4 pilar OOP di halaman ini</div>
        {(cfg.left || []).map((n, i) => <Note key={i} n={n} side="left" />)}
      </aside>
      <aside className="pillar-notes right" aria-hidden>
        {(cfg.right || []).map((n, i) => <Note key={i} n={n} side="right" />)}
      </aside>
    </>
  )
}
