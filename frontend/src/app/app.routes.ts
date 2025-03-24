import { Routes } from '@angular/router';

export const routes: Routes = [
    { path: 'account', loadChildren: () => import('./account/account.module').then(m => m.AccountModule) }
  ];
  