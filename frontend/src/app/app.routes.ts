import { Routes } from '@angular/router';

export const routes: Routes = [
    { path: 'account', loadChildren: () => import('./account/account.module').then(m => m.AccountModule) },
    { path: 'bots', loadChildren: () => import('./bots/bots.module').then(m => m.BotsModule) },
  ];
  